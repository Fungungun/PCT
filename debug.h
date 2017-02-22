#ifndef __COV_DEBUG_H__
#define __COV_DEBUG_H__

#define ERROR_OUT__ std::cerr<<"[ERROR][File:"<<__FILE__<<"][Line:"<<__LINE__<<"]"

#ifndef _DEBUG
#define TB__(A) int64 A; A = cv::getTickCount()
#define TE__(A) std::cout << #A << " : " << 1.E3 * double(cv::getTickCount() - A)/double(cv::getTickFrequency()) << "ms" << std::endl
#else
#define TB__(A)
#define TE__(A)
#endif


/*
 * Du Huynh, Jan 2017.
 */

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <sstream>

using namespace cv;

namespace debug {


    /** return the matrix from the reading of a small text file */
    Mat readTextFile(string filename); 

    /* read in a 17 channel double image from the given file (for testing the
     * covariance matrix computation).
     */
    Mat readFeatureImage(string filename);

    /** print the content of a double image at the specified channel */
    void printDoubleMat(Mat mat, int channel);

    /** print the content of an uchar image at the specified channel */
    void printUcharMat(Mat mat, int channel);

    /** return the content of a double image at the specified channel */
    Mat extractDoubleMat(Mat mat, int channel);

    
}

#endif
