# Frequency Domain Filtering
### Austin Hester
### CS 6420 - Computer Vision
### UMSL SP2021, Prof. Sanjiv Bhatia

----
## Purpose

The purpose of this assignment is to remove periodic noise in an image by filtering in the frequency domain. You will get to
use the OpenCV functions to convert an image into frequency domain by applying Discrete Fourier Transform (DFT), apply the
filter, and convert the filtered image back into spatial domain by applying inverse DFT.

----

## Task

The periodic noise is best removed by filtering an image in the frequency domain. The periodic noise may have been added by
atmospheric disturbance or sensor aberrations. Your task is to remove the periodic noise in a given image.

Assume that you are given an image with periodic noise. Read in the image and convert it to frequency domain. Display the
converted image. Using the mouse, specify the frequencies that need to be remove and change them to zero. Apply inverse
transform to convert the image back in spatial domain and display it.

----

#### Usage

```
Usage: freq_filter.exe [params] input_image output_image

        -b, --blur
                Blur Output Image
        -d, --double
                Double Input Image Size
        -e, --equalize
                Equalize Output Image
        -h, --help (value:true)
                Show Help Message
        -m, --manual
                Manual Filter Mode

        input_image (value:<none>)
                Input Image
        output_image (value:<none>)
                Output Image

```

----

https://github.com/ahester57/frequency_filter
