/*
 * Du Huynh, Jan 2017.
 */

#include <stdio.h>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <assert.h>

using namespace std;
using namespace cv;

namespace debug {

    /** return the uchar matrix from the reading of a small text file */
    Mat readTextFile(string filename) {
        FILE *fp = fopen(filename.c_str(), "r");
        int nRows, nCols, nChannels;
        fscanf(fp, "%d%d%d", &nRows, &nCols, &nChannels);
        assert(nChannels == 1 || nChannels == 3);
        Mat mat;
        if (nChannels == 1)
            mat = Mat(nRows,nCols,CV_8U);
        else
            mat = Mat_<Vec<uchar,3> >(nRows,nCols);

        int intval;
        uchar *ptr;
        for (int ch=0; ch < nChannels; ch++)
            for (int r=0; r < nRows; r++) {
                ptr = mat.ptr(r) + ch;
                for (int c=0; c < nCols; c++, ptr+=nChannels) {
                    fscanf(fp, "%d", &intval);
                    *ptr = (uchar)intval;
                }
            }
        fclose(fp);
        return mat;
    }

    /* ------------------------------------------------------------ */

    /* read in a 17 channel double image from the given file (for testing the
     * covariance matrix computation).
     */
    Mat readFeatureImage(string filename)
    {
        FILE *fp = fopen(filename.c_str(), "r");
        int nRows, nCols, nChannels;
        fscanf(fp, "%d%d%d", &nRows, &nCols, &nChannels);
        assert(nChannels == 7 || nChannels == 17);
        Mat mat;
        if (nChannels == 7) mat = Mat_<Vec<double,7> >(nRows,nCols);
        else mat = Mat_<Vec<double,17> >(nRows,nCols);

        double dval;
        double *ptr;
        for (int ch=0; ch < nChannels; ch++)
            for (int r=0; r < nRows; r++) {
                ptr = (double *)mat.ptr<double>(r) + ch;
                for (int c=0; c < nCols; c++, ptr+=nChannels) {
                    fscanf(fp, "%lf", &dval);
                    *ptr = (double)dval;
                }
            }
        fclose(fp);
        return mat;
        
    }

    /* ------------------------------------------------------------ */

    /** print the content of a double image at the specified channel */
    void printDoubleMat(Mat mat, int channel) {
        assert(channel >= 0 && channel < mat.channels());
        int nChannels = mat.channels();
        for (int r=0; r < mat.rows; r++) {
            double *ptr = (double *)mat.ptr<double>(r) + channel;
            for (int c=0; c < mat.cols; c++, ptr += nChannels)
                cerr << *ptr << " ";
            cerr << endl;
        }
    }

    /* ------------------------------------------------------------ */

    /** print the content of an uchar image at the specified channel */
    void printUcharMat(Mat mat, int channel) {
        assert(channel >= 0 && channel < mat.channels());
        int nChannels = mat.channels();
        for (int r=0; r < mat.rows; r++) {
            uchar *ptr = mat.ptr<uchar>(r) + channel;
            for (int c=0; c < mat.cols; c++, ptr += nChannels)
                cerr << (int)*ptr << " ";
            cerr << endl;
        }
    }


}
