// freq_helper.cpp : This file contains the helper functions for the main
// Austin Hester CS642o mar 2021
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>

#include "./include/freq_helper.hpp"
    #include "./include/dir_func.hpp"
    #include "./include/string_helper.hpp"


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
    cv::Mat planes[] = { cv::Mat_<float>( src ), cv::Mat::zeros( src.size(), CV_32F ) };
    cv::Mat complex_image;
    cv::merge( planes, 2, complex_image );

    return complex_image;
}


// create a normalized magnitude image from complex image
cv::Mat
create_magnitude_image(cv::Mat* src)
{
    // swap quadrants of complex image
    swap_quadrants( src );

    // split swapped complex image into real and imaginary
    cv::Mat planes[2];
    cv::split( *src, planes );

    // compute magnitude
    cv::Mat magnitude_image;
    cv::magnitude( planes[0], planes[1], magnitude_image );

    // normalize magnitude
    cv::normalize( magnitude_image, magnitude_image, 0, 1, cv::NORM_MINMAX);
    magnitude_image.convertTo( magnitude_image, CV_8U, 255 );

    return magnitude_image;
}


// apply thresholded magnitude image to complex image
cv::Mat
apply_magnitude(cv::Mat* src, cv::Mat magnitude)
{
    // threshold the magnitude image so all non-zero pixels are 1
        cv::imshow( " magnitude Image", magnitude );

    cv::Mat thresholded;
    magnitude.copyTo(thresholded);
    cv::threshold( magnitude, thresholded, 0, 1, cv::THRESH_BINARY );

    cv::imshow( " threshold Image", thresholded );
    // multiply planes of complex images by threshold by applying as mask
    cv::Mat planes[2];
    cv::split( *src, planes );

    // planes[0].copyTo( planes[0], cv::Mat::zeros(planes[0].size(), CV_8U) );
    cv::Mat real;
    // cv::bitwise_and( planes[0], cv::Mat::zeros(planes[0].size(), CV_32F), real );
    cv::Mat img;
    cv::bitwise_and( planes[0], cv::Mat_<float>( thresholded ), real );
    cv::bitwise_and( planes[1], cv::Mat_<float>( thresholded ), img );
    write_img_to_file_as_text<float>( cv::Mat_<float>( thresholded ) , "./out", "threhs.tif");

    // and merge them into a new complex image
    cv::Mat planes2[] = { real, img };
    cv::merge( planes2, 2, *src );

    // swap quandrants
    swap_quadrants( src );

    thresholded.release();
    return *src;
}


// create a normalized real plane image from a thresholded complex image
cv::Mat
extract_real_image(cv::Mat src)
{
    // split the complex image into planes
    cv::Mat planes[2];
    cv::split( src, planes );

    cv::Mat normal_real_plane;
    cv::normalize( planes[0], normal_real_plane, 0, 1, cv::NORM_MINMAX );
    normal_real_plane.convertTo( normal_real_plane, CV_8U, 255 );

    return normal_real_plane;
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
