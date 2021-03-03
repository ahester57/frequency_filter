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
create_padded_image(cv::Mat src)
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

    // crop if odd
    padded_image = padded_image(
        cv::Rect( 0, 0, padded_image.cols & -2, padded_image.rows & -2 )
    );

    std::cout << "Padded Image size is:\t\t\t" << padded_image.cols << "x" << padded_image.rows << std::endl;
    return padded_image;
}


// create a complex image from a input image
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


// create a normalized magnitude image from complex image
cv::Mat
create_magnitude_image(cv::Mat src)
{
    // split swapped complex image into real and imaginary
    cv::Mat planes[2];
    cv::split( src, planes );

    // compute magnitude
    cv::Mat magnitude_image;
    cv::magnitude( planes[0], planes[1], magnitude_image );

    // normalize magnitude
    cv::Mat normalized_mag;
    cv::normalize( magnitude_image, normalized_mag, 0, 1, cv::NORM_MINMAX);
    normalized_mag.convertTo( normalized_mag, CV_8U, 255 );

    magnitude_image.release();
    return normalized_mag;
}


// apply thresholded magnitude image to complex image
cv::Mat
apply_magnitude(cv::Mat src, cv::Mat magnitude)
{
    // threshold the magnitude image so all non-zero pixels are 1
    cv::Mat thresholded;
    cv::threshold( magnitude, thresholded, 0, 1, cv::THRESH_BINARY );

    // multiply planes of complex images by threshold by applying as mask
    cv::Mat planes[2];
    cv::split( src, planes );
    planes[0].copyTo( planes[0], thresholded );
    planes[1].copyTo( planes[1], thresholded );

    // and merge them into a new complex image
    cv::Mat complex_image;
    cv::merge( planes, 2, complex_image );

    thresholded.release();
    return complex_image;
}


// swap images
void
swap_mat(cv::Mat* a, cv::Mat* b)
{
    cv::Mat tmp;
    a->copyTo(tmp);
    b->copyTo(*a);
    tmp.copyTo(*b);
    tmp.release();
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
