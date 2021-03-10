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


cv::Mat
filter_frequency_from_image(cv::Mat image)
{
    // initialize images
    cv::Size input_image_size = image.size();

    // make padded image from input image
    cv::Mat padded_image = create_padded_image( image );
    image.release();

    // make complex image from padded image
    cv::Mat complex_image = create_complex_image( padded_image );
    padded_image.release();

    // apply dft on the complex image
    cv::dft( complex_image, complex_image );

    // make magnitude image from complex image
    cv::Mat magnitude_image = create_magnitude_image( &complex_image );

    // display normalized magnitude image
    // cv::imshow( WINDOW_NAME + " Magnitude Image", magnitude_image );

    // filter the periodic noise
    cv::Mat freq_filter_image = create_frequency_mask( magnitude_image );
    magnitude_image.release();

    cv::imshow( WINDOW_NAME + " Frequency Mask", freq_filter_image );
    // write_img_to_file( freq_filter_image, "./out", "freq_mask_" + output_image_filename);

    // 'event loop' for keypresses
    while (wait_key());

    // apply magnitude to new complex image
    complex_image = apply_magnitude( &complex_image, freq_filter_image );
    freq_filter_image.release();

    // apply inverse fourier transform
    cv::idft( complex_image, complex_image );

    // extract real plane and crop to size of original
    cv::Mat normal_real_plane = extract_real_image( complex_image );
        //@ ( cv::Rect( input_image_size, cv::Point(0, 0) ) );

    complex_image.release();
    return normal_real_plane;
}


int
main(int argc, const char** argv)
{
    // CLA variables
    std::string input_image_filename;
    bool double_input_size;
    bool blur_output;
    bool equalize_output;

    // parse and save command line args
    int parse_result = parse_arguments(
        argc, argv,
        &input_image_filename,
        &output_image_filename,
        &double_input_size,
        &blur_output,
        &equalize_output
    );
    if (parse_result != 1) return parse_result;

    cv::Mat input_image = open_image( input_image_filename, true );

    // double the input size if given 'd' flag
    if (double_input_size) {
        cv::resize( input_image, input_image, input_image.size() * 2);
    }

    cv::imshow( WINDOW_NAME + " Input Image", input_image );

    cv::Mat filtered_image = filter_frequency_from_image( input_image );

    // blur the output if given 'b' flag
    if (blur_output) {
        cv::Mat blurred_filtered;
        cv::GaussianBlur( filtered_image, blurred_filtered, cv::Size(3, 3), 3 );
        cv::addWeighted( filtered_image, 1.5, blurred_filtered, -0.3, 0, blurred_filtered );
        cv::equalizeHist( blurred_filtered, filtered_image );
        blurred_filtered.release();
    }

    // equalize the output if given 'e' flag
    if (equalize_output) {
        cv::equalizeHist( filtered_image, filtered_image );
    }

    cv::imshow( WINDOW_NAME + " Fixed Image", filtered_image );
    write_img_to_file( filtered_image, "./out", output_image_filename );

    // 'event loop' for keypresses
    while (wait_key());

    cv::destroyAllWindows();
    filtered_image.release();

    return 0;
}
