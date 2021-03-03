// freq_helper.hpp : Helper Functions
// Austin Hester CS542o mar 2021
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#ifndef FREQ_HELPER_H
#define FREQ_HELPER_H

#include <opencv2/core/core.hpp>

#include <vector>
#include <iostream>


cv::Mat pad_image(cv::Mat src);
cv::Mat create_complex_image(cv::Mat src);
void swap_mat(cv::Mat* a, cv::Mat* b);
void swap_quadrants(cv::Mat* src);

#endif
