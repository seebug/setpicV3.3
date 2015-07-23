#include "ebuscam.h"

#include <cstdlib>
#include <cstdio>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "lscript.h"

int cb_test(EBUS_IMAGE* img)
{
    std::fprintf(stderr, "%s\n","callback=====");
    return 0;
    //cv::Mat _img=cv::Mat(img->Height,img->Width,CV_MAKETYPE(img->Depth,img->Channel),img->pImageBuf);
    //cv::namedWindow("img",0);
    //cv::imshow("img",_img);
    //cv::waitKey(2000);
    //cv::destroyWindow("img");
}

int main(int argc,char* argv[])
{
    std::fprintf(stderr, "%s\n","load_config");
    if(0!=load_config("camera.pvcfg"))
    {
        std::fprintf(stderr, "%s\n","error : load_config");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","restore_config");
    if(0!=restore_config())
    {
        std::fprintf(stderr, "%s\n","error : restore_config");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","open");
    if(0!=open())
    {
        std::fprintf(stderr, "%s\n","error : open");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","set_buffer");
    if(0!=set_buffer())
    {
        std::fprintf(stderr, "%s\n","error : set_buffer");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","set_width");
    if(0!=set_width(2048))
    {
        std::fprintf(stderr, "%s\n","error : set_width");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","set_height");
    if(0!=set_height(1000))
    {
        std::fprintf(stderr, "%s\n","error : set_height");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","set_callback");
    if(0!=set_callback(cb_test))
    {
        std::fprintf(stderr, "%s\n","error : set_callback");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","set_trigger_free");
    if(0!=set_trigger_free())
    {
        std::fprintf(stderr, "%s\n","error : set_trigger_free");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","grab");
    if(0!=grab())
    {
        std::fprintf(stderr, "%s\n","error : grab");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr,"%s\n","sleep ......");
    usleep(5000000);
    long int sr(5000),sg(6000),sb(7000);
    std::fprintf(stderr, "%s\n","set_exposure_abs");
    if(0!=set_exposure_abs(sr,sg,sb))
    {
        std::fprintf(stderr, "%s\n","error : set_exposure_abs");
        return EXIT_FAILURE;
    }
    usleep(5000000);
    long int r(0),g(0),b(0);
    std::fprintf(stderr, "%s\n","get_exposure_abs");
    if(0!=get_exposure_abs(r,g,b))
    {
        std::fprintf(stderr, "%s\n","error : get_exposure_abs");
        return EXIT_FAILURE;
    }

    if(r!=sr || g!=sg || b!=sb)
    {
        std::fprintf(stderr, "%s\n","error : r!=sr || g!=sg || b!=sb");
        std::fprintf(stderr, "r=%lld \tg=%lld \tb=%lld \n",r,b,b);
    }

    usleep(5000000);
    std::fprintf(stderr, "%s\n","stop");
    if(0!=stop())
    {
        std::fprintf(stderr, "%s\n","error: stop");
    }

    std::fprintf(stderr, "%s\n","unset_buffer");
    if(0!=unset_buffer())
    {
        std::fprintf(stderr, "%s\n","error : unset_buffer");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","close");
    if(0!=close())
    {
        std::fprintf(stderr, "%s\n","error : close");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","unload_config");
    if(0!=unload_config())
    {
        std::fprintf(stderr, "%s\n","error : unload_config");
        return EXIT_FAILURE;
    }

    std::fprintf(stderr, "%s\n","test done");
    getchar();

}
