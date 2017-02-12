#ifndef __COV_UTILS_H__
#define __COV_UTILS_H__

#include "covImage.h"
#include "Cparticle.h"
#include "SParater.h"
#include "cpp/INIReader.h"

namespace utils {

    /*
    .....
    */
    void InitPara(Parameter &para);

    /*
    .....
    */
    Mat InitModeTranMat(int nModes);

    /*
    return the ground truth of position
    */

    Mat load_pos_gt(Parameter para);

    /* Given the top-left and bottom-right corner coordinates of a image
    * region, this function returns the coordinates of the the four corners
    * of 4 quadrants.
    * Input arguments:
    * x1, y1, x2, y2 - the top-left (x1,y1) and bottom-right corners of the
    *                  image region.
    * Output arguments:
    * qx1, qy1, qx2, qy2 - the coordinates of the four quadrants. Each of
    *                      these arguments is an array of four
    *                      elements. Element 0 is for the top-left quadrant;
    *                      element 1 is for the top-right quadrant; element 2
    *                      is for the bottom-left quadrant; element 3 is for
    *                      the bottom-right quadrant.
    */
    void getQuadrants(int x1, int y1, int x2, int y2,
        int *qx1, int *qy1, int *qx2, int *qy2);


    /*  ......
     */
    void getVerticalHalf(int x1, int y1, int x2, int y2,
        int *qx1, int *qy1, int *qx2, int *qy2);


    /*  ......
     */
    void getHorizontalHalf(int x1, int y1, int x2, int y2,
        int *qx1, int *qy1, int *qx2, int *qy2);

    /*  ......
     */
    void GenImgName(vector<string> &filename, Parameter para);

    /*  ......
     */
    Mat  GenParticlePostion(Mat meanm, Mat stddevm, int nParticles);
    /* 
        normalize the weight of particles
    */
    void mode_tran(Mat &par_wt, Mat sum_wt, Parameter &para);

    /*  
     * generate different modes
     */
    vector<vector<int>> CovmatQuadrantRef(int modes);

    /*
    ...
     */
    Mat SearchParticle(CovImage covimg, Cparticle tarpar, Parameter &para, Mat pos_gt);

    /*  
    ...
     */
    void ShowResults(CovImage covimg, string filename , Mat final_pos, Parameter &para);

    /*  
    ...
     */
    vector<vector<int>> ModeQuadrantRef(int nModes);

    /*  
    ...
     */
    vector<Point> calcpoint_draw(int nModes);

    /*  
    ...
     */
    void updateStddev(Mat &stddev, Parameter &para, Mat tarpos);

};

#endif