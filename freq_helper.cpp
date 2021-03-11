// freq_helper.cpp : This file contains the helper functions for the main
// Austin Hester CS642o mar 2021
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>

#include "./include/freq_helper.hpp"
    // #include "./include/dir_func.hpp"
    // #include "./include/string_helper.hpp"


// event loop
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
    cv::Mat thresholded;
    cv::threshold( magnitude, thresholded, 0, 1, cv::THRESH_BINARY );

    // multiply planes of complex images by threshold by applying as mask
    cv::Mat planes[2];
    cv::split( *src, planes );

    cv::Mat real;
    planes[0].copyTo( real , thresholded );
    cv::Mat img;
    planes[1].copyTo( img, thresholded );
    // write_img_to_file_as_text<float>( cv::Mat_<float>( thresholded ) , "./out", "threhs.tif");

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


// draw contours of canny edge detection
cv::Mat
draw_canny_contours(cv::Mat magnitude_image)
{
    cv::Mat canny_output;
    magnitude_image.copyTo( canny_output );
    cv::Canny( canny_output, canny_output, 10, 15 );

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours( canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE );

    cv::Mat canvas = cv::Mat::zeros( canny_output.size(), CV_8U );
    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours( canvas, contours, i, cv::Scalar(255), cv::FILLED, cv::LINE_8, hierarchy, 0 );
    }

    // cv::imshow(" Contours Image", canvas );

    canny_output.release();
    return canvas;
}


// remove periodic noise from a frequency magnitude image
cv::Mat
auto_filter(cv::Mat magnitude_image)
{
    // draw canny contours
    cv::Mat canny_output = draw_canny_contours( magnitude_image );
    // create contour mask (which contours to get rid of)
    cv::Mat mask = cv::Mat( canny_output.size(), CV_8U );
    mask = cv::Scalar::all(255);
    // exclude the middle of the mask
    cv::circle( mask, cv::Point( mask.cols/2, mask.rows/2 ), 2, cv::Scalar(0), cv::FILLED );
    // apply the mask to magnitude image
    cv::bitwise_and( canny_output, mask, canny_output );
    mask.release();
    // return the new magnitude image, outer frequencies filtered
    cv::bitwise_not( canny_output, canny_output );
    return canny_output;
}


// remove periodic noise manually
cv::Mat
manual_filter(cv::Mat magnitude_image)
{
    // display normalized magnitude image
    // cv::threshold( magnitude_image, magnitude_image, 1, 255, cv::THRESH_BINARY );
    cv::imshow( "Magnitude Image", magnitude_image );
    // initialize mask
    cv::Mat mask = cv::Mat( magnitude_image.size(), CV_8U );
    mask = cv::Scalar::all(255);
    // initialize mouse callback
    init_callback( "Magnitude Image", &mask );
    // wait for q
    while (wait_key());
    return mask;
}


// draw zeros on image
void
mouse_callback_draw_zeros(int event, int x, int y, int d, void* userdata)
{
    cv::Mat* frequency_mask = (cv::Mat*) userdata;

    switch (event) {
        case cv::EVENT_LBUTTONUP:
            // push the new point
            // draw a circle mask at chosen points
            cv::circle( *frequency_mask, cv::Point2f( x, y ), 4, cv::Scalar(0), cv::FILLED );
            // show the copy of the image
            cv::imshow( "tmp:", *frequency_mask );
            break;
    }
}


// assign mouse callbacks
void
init_callback(std::string window_name, cv::Mat* frequency_mask)
{
    cv::setMouseCallback( window_name, mouse_callback_draw_zeros, frequency_mask );
}
