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
#include "./include/freq_helper.hpp"

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
    cv::Mat input_image = open_image( input_image_filename, true );

    cv::imshow( WINDOW_NAME + " Input Image", input_image );

    // make padded image from input image
    cv::Mat padded_image = create_padded_image( input_image );

    // make complex image from padded image
    cv::Mat complex_image = create_complex_image( padded_image );

    // apply dft on the complex image
    cv::dft( complex_image, complex_image );

    // swap quadrants of complex image
    swap_quadrants( &complex_image );

    // make magnitude image from complex image
    cv::Mat magnitude_image = create_magnitude_image( complex_image );

    // display normalized magnitude image
    cv::imshow( WINDOW_NAME + " Magnitude Image", magnitude_image );

//TODO filter the periodic noise

    // 'event loop' for keypresses
    while (wait_key());

    // apply magnitude to new complex image
    cv::Mat new_complex_image = apply_magnitude( complex_image, magnitude_image );

    // swap quandrants
    swap_quadrants( &new_complex_image );

    // apply inverse fourier transform
    cv::idft( new_complex_image, new_complex_image );

    cv::Mat planes[2];
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
    magnitude_image.release();
    new_complex_image.release();
    normal_real_plane.release();

    return 0;
}
