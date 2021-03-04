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

std::string output_image_filename;


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


// draw contours of canny edge detection
cv::Mat
draw_canny_contours(cv::Mat magnitude_image)
{
    cv::Mat canny_output;
    magnitude_image.copyTo( canny_output );
    // cv::blur( magnitude_image, canny_output, cv::Size(1,1) );
    cv::Canny( canny_output, canny_output, 0, 1 );

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours( canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE );

    cv::Mat canvas = cv::Mat::zeros( canny_output.size(), CV_8U );
    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours( canvas, contours, i, cv::Scalar(255), cv::FILLED, cv::LINE_8, hierarchy, 0 );
    }

    cv::imshow( WINDOW_NAME + " Contours Image", canvas );
    write_img_to_file( canvas, "./out", "contours_" + output_image_filename);

    canny_output.release();
    return canvas;
}


//
cv::Mat
frequency_filter(cv::Mat magnitude_image)
{
    cv::Mat canny_output = draw_canny_contours( magnitude_image );
    cv::Mat mask = cv::Mat::ones( canny_output.size(), CV_8U );
    cv::circle( mask, cv::Point( mask.cols/2, mask.rows/2 ), 75, cv::Scalar(0), cv::FILLED );
    cv::circle( mask, cv::Point( mask.cols/2, mask.rows/2 ), 10, cv::Scalar(255), cv::FILLED );
    cv::imshow( WINDOW_NAME + " af Image", mask );

    // apply the mask to magnitude image
    cv::bitwise_and( canny_output, mask, canny_output );

    mask.release();
    return canny_output;
}


int
main(int argc, const char** argv)
{
    // CLA variables
    std::string input_image_filename;

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
    input_image.release();

    // make complex image from padded image
    cv::Mat complex_image = create_complex_image( padded_image );
    padded_image.release();

    // apply dft on the complex image
    cv::dft( complex_image, complex_image );

    // make magnitude image from complex image
    cv::Mat magnitude_image = create_magnitude_image( &complex_image );

    // display normalized magnitude image
    cv::imshow( WINDOW_NAME + " Magnitude Image", magnitude_image );

    // filter the periodic noise
    cv::Mat freq_filter_image = frequency_filter( magnitude_image );
    magnitude_image.release();

    cv::imshow( WINDOW_NAME + " Frequency Mask", freq_filter_image );
    write_img_to_file( freq_filter_image, "./out", "freq_mask_" + output_image_filename);

    // 'event loop' for keypresses
    while (wait_key());

    // apply magnitude to new complex image
    // cv::Mat planes[2];
    // cv::split( complex_image, planes );
    // cv::imshow("1", planes[0]);
    complex_image = apply_magnitude( &complex_image, freq_filter_image );
    // cv::split( new_complex_image, planes );
    // cv::imshow("2", planes[0]);
    freq_filter_image.release();

            // // make magnitude image from complex image
            //     magnitude_image = create_magnitude_image( new_complex_image );
            //         freq_filter_image = frequency_filter( magnitude_image );

            //     new_complex_image = apply_magnitude( new_complex_image, freq_filter_image );

            // //     // display normalized magnitude image
            //     cv::imshow( WINDOW_NAME + " Magnitude Image2", magnitude_image );

            // //     // filter the periodic noise
            // //     freq_filter_image = frequency_filter( magnitude_image );
            // //     magnitude_image.release();

            //     cv::imshow( WINDOW_NAME + " Frequency Mask2", freq_filter_image );

    // apply inverse fourier transform
    cv::idft( complex_image, complex_image );

    cv::Mat normal_real_plane = extract_real_image( complex_image );
    complex_image.release();

    cv::imshow( WINDOW_NAME + " Fixed Image", normal_real_plane );
    write_img_to_file( normal_real_plane, "./out", output_image_filename );

    // 'event loop' for keypresses
    while (wait_key());

    cv::destroyAllWindows();
    normal_real_plane.release();

    return 0;
}
