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

#include <omp.h>

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

 private:
    /* input image */
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
        cerr << "constructor 1\n";
        im = imread(filename, -1);
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

    /* *** the following functions should be changed to 'private' after
     *  debugging is finished
     */

    /* this function contains a long sequence of operations. It is called by
     *  the constructor.
     */
    void process() {
        nChannels = im.channels();
        nRows = im.rows;
        nCols = im.cols;
        dim = nChannels*5 + 2;

        cerr << "nRows = " << nRows << " Ncols = " << nCols;
        cerr << " dim = " << dim << "\n";

        assert(dim == FEAT_DIM1 || dim == FEAT_DIM3);

        // initialize featimage
        if (dim == FEAT_DIM1)
            featimage = Mat_<Vec<double,FEAT_DIM1> >(nRows, nCols);
        else
            featimage = Mat_<Vec<double,FEAT_DIM3> >(nRows, nCols);

        coordinateX();
        cerr << "coordinateX() done\n";
        debug::printDoubleMat(featimage, 0);

        coordinateY();
        cerr << "coordinateY() done\n";
        debug::printDoubleMat(featimage, 1);

        for (int c=0; c < nChannels; c++) {
            intensity(c);
            cerr << "intensity() channel " << c << " done\n";
            debug::printDoubleMat(featimage, 2+c);
            gradientX(c);
            cerr << "gradientX() channel " << c << " done\n";
            debug::printDoubleMat(featimage, 2+nChannels+c);
            gradientY(c);  // should be in channel 2+2*nChannels+c
            cerr << "gradientY() channel " << c << " done\n";
            debug::printDoubleMat(featimage, 2+2*nChannels+c);
            gradient2X(c); // channel 2+3*nChannels+c
            cerr << "gradient2X() channel " << c << " done\n";
            debug::printDoubleMat(featimage, 2+3*nChannels+c);
            gradient2Y(c); // channel 2+4*nChannels+c
            cerr << "gradient2Y() channel " << c << " done\n";
            debug::printDoubleMat(featimage, 2+4*nChannels+c);
        }

        computeIntegralImage();
    }

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
        for (int i=1; i <= S; i++) s += i;
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
    void covComponentMatrices(double x1, double y1, double x2, double y2,
                              Mat &prodM, Mat &sumM, double &N);

    /* return the covariance of the region bounded by (x1,y1) and (x2,y2)
     * where (x1,y1) and (x2,y2) should be the top-left and bottom-right
     * corners of the region. The total number of pixels is returned to the
     * last argument.
     */
    Mat covMatrix(double x1, double y1, double x2, double y2, double &Npixels);
};

