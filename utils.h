#ifndef __COV_UTILS_H__
#define __COV_UTILS_H__

#include "covImage.h"
#include "Cparticle.h"

namespace utils {
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
    void GenImgName(string *filename, string file,int framelength, int dataset);

    /*  ......
     */
    Mat  GenParticlePostion(Mat meanm, Mat stddevm, int nParticles);
     /* 
         normalize the weight of particles
     */
    void normWt(Mat &par_wt, Mat sum_wt, int &finalmode);

    /*  
        generate different modes
     */
    vector<vector<int>> CovmatQuadrantRef(int modes);

    /*
        ...
    */
    Mat SearchParticle(CovImage covimg, Cparticle tarpar, int nParticles, int nModes, vector<vector<int>> v, int &mode);
    
    /*  
        ...
    */
    void ShowResults(CovImage covimg, string filename , Mat final_pos, string file, int nParticles, int mode, vector<vector<int>> v_draw);
    
    /*  
        ...
    */
    vector<vector<int>> ModeQuadrantRef(int nModes);
    /*  
        ...
    */
    vector<Point> calcpoint_draw(int nModes);

};

#endif