// freq_helper.cpp : This file contains the helper functions for the main
// Austin Hester CS642o mar 2021
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>

#include "./include/freq_helper.hpp"


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


cv::Mat
create_complex_image(cv::Mat src)
{
    // make floating point images, same size as padded, of type float
    // copy input image to real part, leaving imaginary blank
    cv::Mat real_part = cv::Mat_<float>( src );
    cv::Mat imaginary_part = cv::Mat::zeros( src.size(), CV_32F );

    // make a complex image using the real and imaginary parts
    cv::Mat planes[] = { real_part, imaginary_part };
    cv::Mat complex_image;
    cv::merge( planes, 2, complex_image );

    real_part.release();
    imaginary_part.release();
    return complex_image;
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


// swap quandrants of an image
void
swap_quadrants(cv::Mat* src)
{
    // assert cols and rows are even
    assert( !( src->cols & 1 || src->rows & 1) );
    int c_x = src->cols / 2;
    int c_y = src->rows / 2;

    cv::Mat q0( *src, cv::Rect( 0, 0, c_x, c_y )); // top_left
    cv::Mat q1( *src, cv::Rect( c_x, 0, c_x, c_y )); // top_right
    cv::Mat q2( *src, cv::Rect( 0, c_y, c_x, c_y )); // bottom_left
    cv::Mat q3( *src, cv::Rect( c_x, c_y, c_x, c_y )); // bottom_right

    // swap quandrants
    swap_mat( &q0, &q3 );
    swap_mat( &q1, &q2 );

    q0.release();
    q1.release();
    q2.release();
    q3.release();
}
