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
    cv::Mat input_image = open_image(input_image_filename);

    // begin image registration by displaying input
    cv::imshow( WINDOW_NAME + " Input Image", input_image );

    // 'event loop' for keypresses
    while (wait_key());

    cv::destroyAllWindows();
    input_image.release();

    return 0;
}
