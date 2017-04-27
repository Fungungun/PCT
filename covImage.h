#ifndef __COV_IMAGE_H__
#define __COV_IMAGE_H__
/*
 * Chenghuan, Du Huynh, Jan 2017.
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <time.h>
#include <sstream>
#include <string>
#include <assert.h>

#include "debug.h"

/* feature dimension of single-channel and 3-channel images */
#define FEAT_DIM1 7
#define FEAT_DIM3 17

/* number of components in the integral image for single-channel and 3-channel
 * images.
 *   II_DIM1 = 1 + 2 + ... + FEAT_DIM1
 *   II_DIM3 = 1 + 2 + ... + FEAT_DIM3
 */
#define II_DIM1 28
#define II_DIM3 153

using namespace std;
using namespace cv;

class CovImage {

 public:
    /*search area*/
    vector<double> mBoundary;

    /*input image*/
    Mat im_in;    
    /*image in Lab space*/
    Mat im;


    /* #channels of im */
    int nChannels;
    /* #rows of im */
    int nRows;
    /* #columns of im */
    int nCols;


    /* featimage stores the feature vectors of all the pixels. This is an
     * nRows x nCols x dim image. The components of the feature vector are: x,
     * y, I, Ix, Iy, Ixx, Iyy. Each of the last 5 components may repeat 3
     * times if nChannels=3. Note that dim must be equal to FEAT_DIM1 or
     * FEAT_DIM3.
     */
    Mat featimage;
    /* dimension of the feature vectors. The covariance matrix is dim x dim.
     * This is also the step between adjacent pixels.
     */
    int dim;
    /* the integral image consists of two parts: (1) IIprod is an nRows x
     * nCols x L image where L = 1 + 2 + ... + dim storing the cumulative
     * product of the different components with each other. (2) IIsum is an
     * nRows x nCols x dim image storing the cumulative sum of the
     * components. Both matrices work together as a Look-Up-Table (LUT) for
     * the upper triangular part of the covariance matrix of a given image
     * region.
     */
    Mat IIprod;
    Mat IIsum;

public:

    /* Constructor. Read in a greyscale or colour image stored in the file
     * and construct a CovImage object.
     * Input parameter:
     *   filename - the name of the image file.
     */
    CovImage(string filename) {
        im_in = imread(filename, -1);
        imin_rgb2lab();
       process();
    }

    /* Constructor. Read in a greyscale or colour image stored in the file
     * and construct a CovImage object 
            ****of only a small search area around the target. ****
     * Input parameter:
     *   filename - the name of the image file.
     *   tarpos   - position of the target in last frame
     */
    CovImage(string filename, float SearchAreaMargin, Mat &tarpos) {
        im_in = imread(filename, -1);
        imin_rgb2lab();
        SetSearchArea(SearchAreaMargin, tarpos);
        //cout<<tarpos<<endl;
        //cout<<mBoundary[0]<<mBoundary[1]<<mBoundary[2]<<mBoundary[3]<<endl;
        //cout<<SearchAreaMargin<<endl;
       process();
    }


    /* Constructor. Read in a greyscale image stored in text format in the
     * given file. This constructor should be used for debugging purpose.
     * Input parameters:
     *   filename - the name of the image file.
     *   flag     - an integer of any value.
     */
    CovImage(string filename, int flag) {
        cerr << "constructor 2\n";
        im = debug::readTextFile(filename);
        process();
    }


    /* Constructor. This constructor can be used for directly debugging,
     * e.g. to test whether the integral image and covariance matrix are
     * correctly computed. A small synthesized feature image can be passed
     * directly to the constructor. A CovImage object created using this
     * constructor does not have the following instance variable defined: im,
     * nChannels.
     * Input parameter:
     *   inputMat - the input matrix. It must be of type double and its number
     *              of channels must be equal to FEAT_DIM1 or FEAT_DIM2.
     */
    CovImage(Mat inputMat) {
        cerr << "constructor 3\n";
        featimage = inputMat;
        nRows = featimage.rows;
        nCols = featimage.cols;
        dim = featimage.channels();
        computeIntegralImage();
    }

    /* destructor */
    ~CovImage() {}

public:
/*  Set the search area */
    void SetSearchArea(float SearchAreaMargin, Mat &tarpos);
/*  convert default rgb image to CV_64F Lab image */
    void imin_rgb2lab(); 
/* this function contains a long sequence of operations. It is called by the constructor.*/
    void process();

public:
    /* ...... */
    void coordinateX();

    /* .... */
    void coordinateY();

    /* ......  */
    void intensity(int channel);

    /* compute the gradient for the X-direction for the specified channel */
    void gradientX(int channel);

    /* compute the gradient for the Y-direction for the specified channel */
    void gradientY(int channel);
 
    /* compute the gradient for the X-direction for the specified channel */
    void gradient2X(int channel);

    /* compute the gradient for the Y-direction for the specified channel */
    void gradient2Y(int channel);

    void computeIntegralImage();


 public:
    /* return the total from 1 to S inclusive */
    inline int total(int S) {
        int s = 0;
        for (int i=1; i <= S; i++) {s += i;}
        return s;
    }

    /* return the number of rows of the input image */
    int getnRows() {
        return nRows;
    }

    /* return the number of columns of the input image */
    int getnCols() {
        return nCols;
    }

    /* return the number of channels of the input image */
    int getnChannels() {
        return nChannels;
    }

    /* return the featimage matrix held in the object */
    Mat getfeatimage() {
        return featimage;
    }

     /* bilinearly interpolates the integral image to get the IIprod vector at
     * the subpixel coordinates (x,y). This function should be used for
     * greyscale images.
     * Input parameters:
     *   (x,y) - the subpixel coordinates.
     * The function outputs a Vec<double,II_DIM1> object.
     */
    Vec<double,II_DIM1> interpIIprod1(double x, double y);

    /* bilinearly interpolates the integral image to get the IIprod vector at
     * the subpixel coordinates (x,y). This function should be used for colour
     * images.
     * Input parameters:
     *   (x,y) - the subpixel coordinates.
     * The function outputs a Vec<double,II_DIM3> object.
     */
    Vec<double,II_DIM3> interpIIprod3(double x, double y);

    /* bilinearly interpolates the integral image to get the IIsum vector
     * at the subpixel coordinates (x,y).  This function should be used for
     * greyscale images.
     * Input parameters:
     *   (x,y) - the subpixel coordinates.
     * The function outputs either a Vec<double,FEAT_DIM1> or 
     * Vec<double,FEAT_DIM3> object.
     */
    Vec<double,FEAT_DIM1> interpIIsum1(double x, double y);

    /* bilinearly interpolates the integral image to get the IIsum vector at
     * the subpixel coordinates (x,y).  This function should be used for
     * greyscale images.
     * Input parameters:
     *   (x,y) - the subpixel coordinates.
     * The function outputs either a Vec<double,FEAT_DIM1> or 
     * Vec<double,FEAT_DIM3> object.
     */
    Vec<double,FEAT_DIM3> interpIIsum3(double x, double y);
    /* return the component matrices that can be used to reconstruct the
     * covariance matrix of the region bounded by (x1,y1) and (x2,y2) where
     * (x1,y1) and (x2,y2) should be the top-left and bottom-right corners of
     * the region. The last parameter is the total number of pixels in the
     * region.
     *
     * The output matrix prodM should be dim x dim in size.
     * The output matrix sumM should be dim x 1 in size.
     *
     * The covariance matrix can be reconstructed as:
     *    prodM / N - sumM*sumM' / N^2
     * or
     *    prodM / (N-1) - sumM*sumM' / (N*(N-1))
     */
//     void covComponentMatrices(int x1, int y1, int x2, int y2,
//         Mat &prodM, Mat &sumM, double &Npixels);
  
    

    /* return the covariance of the region bounded by (x1,y1) and (x2,y2)
     * where (x1,y1) and (x2,y2) should be the top-left and bottom-right
     * corners of the region. The total number of pixels is returned to the
     * last argument.
     */
//    Mat covMatrix(int x1, int y1, int x2, int y2, int &Npixels);

    //overload function for intepolating method
    void covComponentMatrices(double x1, double y1, double x2, double y2,
        Mat &prodM, Mat &sumM, double &Npixels);
    Mat covMatrix(double x1, double y1, double x2, double y2, double &Npixels);
};

#endif