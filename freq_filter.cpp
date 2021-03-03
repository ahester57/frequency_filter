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


// swap quandrants
void
swap_mat(cv::Mat* a, cv::Mat* b)
{
    cv::Mat tmp;
    a->copyTo(tmp);
    b->copyTo(*a);
    tmp.copyTo(*b);
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

    cv::imshow( WINDOW_NAME + " Input Image", input_image );

    // make padded image
    cv::Mat padded_image = pad_image( input_image );

    // crop if odd
    padded_image = padded_image(
        cv::Rect( 0, 0, padded_image.cols & -2, padded_image.rows & -2 )
    );

    // make floating point images, same size as padded, of type float
    // copy input image to real part, leaving imaginary blank
    cv::Mat real_part = cv::Mat_<float>( padded_image );
    cv::Mat imaginary_part = cv::Mat::zeros( padded_image.size(), CV_32F );

    // make a complex image using the real and imaginary parts
    cv::Mat planes[] = { real_part, imaginary_part };
    cv::Mat complex_image;
    cv::merge( planes, 2, complex_image );

    // apply dft on the complex image
    cv::dft( complex_image, complex_image );

    int center_x = complex_image.cols / 2;
    int center_y = complex_image.rows / 2;

    cv::Mat q0( complex_image, cv::Rect( 0, 0, center_x, center_y )); // top_left
    cv::Mat q1( complex_image, cv::Rect( center_x, 0, center_x, center_y )); // top_right
    cv::Mat q2( complex_image, cv::Rect( 0, center_y, center_x, center_y )); // bottom_left
    cv::Mat q3( complex_image, cv::Rect( center_x, center_y, center_x, center_y )); // bottom_right

    // swap quandrants
    swap_mat( &q0, &q3 );
    swap_mat( &q1, &q2 );

    // split swapped complex image into real and imaginary
    cv::split( complex_image, planes );

    // compute magnitude
    cv::Mat magnitude_image;
    cv::magnitude( planes[0], planes[1], magnitude_image );

    // normalize magnitude
    cv::Mat normalized_mag;
    cv::normalize( magnitude_image, normalized_mag, 0, 1, cv::NORM_MINMAX);
    normalized_mag.convertTo( normalized_mag, CV_8U, 255 );

    // display normalized magnitude image
    cv::imshow( WINDOW_NAME + " Magnitude Image", normalized_mag );

    // 'event loop' for keypresses
    while (wait_key());

    // threshold the image so all non-zero pixels are 1
    cv::Mat thresholded;
    cv::threshold( normalized_mag, thresholded, 0, 1, cv::THRESH_BINARY );

    // write_img_to_file_as_text<uint>( thresholded, "./out", output_image_filename );

    // multiply planes of complex images by threshold by applying as mask
    planes[0].copyTo( planes[0], thresholded );
    planes[1].copyTo( planes[1], thresholded );

    // and merge them into a new complex image
    cv::Mat new_complex_image;
    cv::merge( planes, 2, new_complex_image );

    // swap quandrants
    q0 = cv::Mat( new_complex_image, cv::Rect( 0, 0, center_x, center_y )); // top_left
    q1 = cv::Mat( new_complex_image, cv::Rect( center_x, 0, center_x, center_y )); // top_right
    q2 = cv::Mat( new_complex_image, cv::Rect( 0, center_y, center_x, center_y )); // bottom_left
    q3 = cv::Mat( new_complex_image, cv::Rect( center_x, center_y, center_x, center_y )); // bottom_right

    swap_mat( &q0, &q3 );
    swap_mat( &q1, &q2 );
    cv::idft( new_complex_image, new_complex_image );

    cv::split( new_complex_image, planes );

    cv::Mat normal_real_plane;
    cv::normalize( planes[0], normal_real_plane, 0, 1, cv::NORM_MINMAX);
    normal_real_plane.convertTo( normal_real_plane, CV_8U, 255 );

    cv::imshow( WINDOW_NAME + " Fixed Image", normal_real_plane );
    write_img_to_file( normal_real_plane, "./out", output_image_filename );

    // 'event loop' for keypresses
    while (wait_key());

    cv::destroyAllWindows();
    input_image.release();
    padded_image.release();
    real_part.release();
    imaginary_part.release();
    magnitude_image.release();
    normalized_mag.release();
    thresholded.release();
    new_complex_image.release();
    q0.release();
    q1.release();
    q2.release();
    q3.release();

    return 0;
}
