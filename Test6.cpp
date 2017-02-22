/* A small test program.

Chenghuan Liu , Du Huynh, Jan 2017.

*/

#include "Test6.h"



int main( int argc, char** argv )
{
    vector<string> video_list;
    utils::LoadVideoList(video_list);

    Parameter para;
    for(int video_count = 0; video_count < video_list.size(); ++video_count){
        //load parameters from config.ini
        para.file = video_list[video_count];
        utils::InitPara(para);

        //load ground truth of position
        Mat pos_gt = utils::LoadPosGT(para);
        para.nModes = utils::updateModeNum(pos_gt.row(para.startFrame-2));

        //initialization
        vector<string> filename(para.endFrame);
        utils::GenImgName(filename,para);

        //create target with template library
        Cparticle tarpar(filename,para,pos_gt);

        //write results to file
        ofstream  presults;
        presults.open(".//results//" + para.file + ".txt");
        presults<<para.file<<" "<<para.dataset<<endl;

        //tracking start
        for(int i = para.startFrame - 1; i < para.endFrame ; ++i){
            //load new frame 
            CovImage covimg(filename[i]);
            cout<<"Tracking Frame "<<i+1<<"..."<<endl;
            //search
            tarpar.m_pos = utils::SearchParticle(covimg,tarpar,para,pos_gt.row(i));
            //model update
            if(para.updateFreq != 0){
                if(para.currentMode == 0 && i % para.updateFreq == 0){
                    tarpar.updateModel(covimg,para);
                }
            }        
            //show results 
            utils::ShowResults(covimg,filename[i],tarpar.m_pos, para, pos_gt.row(i));
            //write results to file
            presults<<i+1<<" "<<para.currentMode+1<<" "<<tarpar.m_pos<<endl;
        }
        presults.close();
        destroyAllWindows();
        
    }
    //system("shutdown -h");
    
}
