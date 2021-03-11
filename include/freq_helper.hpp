// freq_helper.hpp : Helper Functions
// Austin Hester CS542o mar 2021
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#ifndef FREQ_HELPER_H
#define FREQ_HELPER_H

#include <opencv2/core/core.hpp>

#include <vector>
#include <iostream>

int wait_key();
cv::Mat create_padded_image(cv::Mat src);
cv::Mat create_complex_image(cv::Mat src);
cv::Mat create_magnitude_image(cv::Mat* src);
cv::Mat apply_magnitude(cv::Mat* src, cv::Mat magnitude);
cv::Mat extract_real_image(cv::Mat src);
void swap_mat(cv::Mat* a, cv::Mat* b);
void swap_quadrants(cv::Mat* src);
cv::Mat draw_canny_contours(cv::Mat magnitude_image);
cv::Mat auto_filter(cv::Mat magnitude_image);
cv::Mat manual_filter(cv::Mat magnitude_image);
void mouse_callback_draw_zeros(int event, int x, int y, int d, void* userdata);
void init_callback(std::string window_name, cv::Mat* frequency_mask);

#endif
