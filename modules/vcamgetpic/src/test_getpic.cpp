#include <cstdlib>
#include <cstdio>
#include <vector>

#include <unistd.h>
#include <signal.h>

#include <opencv2/opencv.hpp>

#include "getpic.h"

bool bExit(false);

void on_signal(int sig)
{
    std::fprintf(stderr, "%s\n","\n\nWill Exit ... ");
    bExit=true;
}

int main(int argc,char* argv[])
{
    signal(SIGINT,on_signal);
    signal(SIGQUIT,on_signal);
    signal(SIGTERM,on_signal);
    char* lpszRt=const_cast<char*>("./runtime"); //const char* => char*
    if(!init_getpic(lpszRt))
    {
        std::fprintf(stderr, "%s\n","init getpic error!");
        return EXIT_FAILURE;
    }
    else
    {
        if(init_shared_memory(2))//count of shared memory
        {
            cv::namedWindow("getpic",0);
            cv::namedWindow("getpic_src",0);
            while(!bExit)
            {
                cv::Mat _img;
                cv::Mat _img_src;
                int total(0);
                //int total(0);
                int classid(0);
                int grab_time(0);
                unsigned long long int ulldTimeStamp;
                cv::Rect outLeft;
                cv::Rect outMiddle;
                cv::Rect outRight;
                int nDataCorrct(0);
                int nDataInit(0);
                int nTriggerMode(0);

                if(get_image(_img_src,_img,total,classid,grab_time,ulldTimeStamp,&outLeft,&outMiddle,&outRight,nDataCorrct,nDataInit,nTriggerMode))
                {
                    std::fprintf(stderr, "total %d  classid:%d  grab_time:%d timestamp:%lld \n", total,classid,grab_time,ulldTimeStamp);
                    std::fprintf(stderr, "left:%d %d %d %d \n",outLeft.x,outLeft.y,outLeft.width,outLeft.height);
                    std::fprintf(stderr, "middle:%d %d %d %d \n",outMiddle.x,outMiddle.y,outMiddle.width,outMiddle.height);
                    std::fprintf(stderr, "right:%d %d %d %d \n",outRight.x,outRight.y,outRight.width,outRight.height);
                    std::fprintf(stderr, "data_corrct %d data_init %d nTriggerMode %d\n",nDataCorrct,nDataCorrct,nTriggerMode);
                    cv::imshow("getpic",_img);
                    cv::imshow("getpic_src",_img_src);
                    cv::waitKey(20);
                }
                usleep(35000);
            }
            cv::destroyWindow("getpic");
            cv::destroyWindow("getpic_src");
            uninit_shared_memory();
        }
        else
        {
            std::fprintf(stderr, "%s\n","init_shared_memory failed !");
        }
    }
    return EXIT_SUCCESS;

}