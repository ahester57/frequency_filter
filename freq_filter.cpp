// freq_filter.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Austin Hester CS642o mar 2021
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>

#include "./include/cla_parse.hpp"
#include "./include/dir_func.hpp"

#define DEBUG 1

const std::string WINDOW_NAME = "Frequency Domain Filtering";


// 'event loop' for keypresses
// call in a while loop to only register q or <esc>
int
wait_key()
{
    char key_pressed = cv::waitKey(0) & 255;
    // 'q' or  <escape> quits out
    if (key_pressed == 27 || key_pressed == 'q') {
        return 0;
    }
    return 1;
}


// Pad image for the optimal DFT size
cv::Mat
pad_image(cv::Mat src)
{
    int m = cv::getOptimalDFTSize( src.rows );
    int n = cv::getOptimalDFTSize( src.cols );

    // make padded image
    cv::Mat padded_image;
    cv::copyMakeBorder(
        src,
        padded_image,
        0, m - src.rows,
        0, n - src.cols,
        cv::BORDER_CONSTANT,
        cv::Scalar::all(0)
    );
    std::cout << "Padded Image size is:\t\t\t" << padded_image.cols << "x" << padded_image.rows << std::endl;
    return padded_image;
}


int
main(int argc, const char** argv)
{
    // CLA variables
    std::string input_image_filename;
    std::string output_image_filename;

    // parse and save command line args
    int parse_result = parse_arguments(
        argc, argv,
        &input_image_filename,
        &output_image_filename
    );
    if (parse_result != 1) return parse_result;

    // initialize images
    cv::Mat input_image = open_image(input_image_filename, true);

    // make padded image
    cv::Mat padded_image = pad_image( input_image );

    // make floating point images, same size as padded, of type float
    // copy input image to real part, leaving imaginary blank
    cv::Mat real_part = cv::Mat_<float>( padded_image );
    cv::Mat imaginary_part = cv::Mat::zeros( padded_image.size(), CV_32F );

    cv::Mat planes[] = { real_part, imaginary_part };

    cv::Mat complex_image;

    cv::merge( planes, 2, complex_image );

    cv::dft( complex_image, complex_image );

    cv::split( complex_image, planes );

    cv::magnitude( planes[0], planes[1], planes[0] );

    cv::Mat magnitude_image = planes[0];

    magnitude_image += cv::Scalar::all(1);
    cv::log( magnitude_image, magnitude_image );

    // crop if odd
    magnitude_image = magnitude_image(
        cv::Rect( 0, 0, magnitude_image.cols & -2, magnitude_image.rows & -2 )
    );

    // begin image registration by displaying input
    cv::imshow( WINDOW_NAME + " Input Image", magnitude_image );

    write_img_to_file( padded_image, "./out", output_image_filename );

    // 'event loop' for keypresses
    while (wait_key());

    cv::destroyAllWindows();
    input_image.release();
    padded_image.release();
    real_part.release();
    imaginary_part.release();

    return 0;
}
