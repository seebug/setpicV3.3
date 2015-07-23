#include "callback.h"
#include "lscript.h"
#include "global.h"
#include "linesf.h"
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

class setpic_timer
{
public:
    setpic_timer(void);
    virtual ~setpic_timer(void) throw();

    uint64_t ticks();
    void restart();

private:
    boost::posix_time::ptime        _start_time;
};

setpic_timer::setpic_timer()
            :_start_time(boost::posix_time::microsec_clock::local_time())
{

}

setpic_timer::~setpic_timer() throw()
{

}

uint64_t setpic_timer::ticks()
{
    boost::posix_time::ptime _current_time(boost::posix_time::microsec_clock::local_time());
    return static_cast<uint64_t>(boost::posix_time::time_duration(_current_time-_start_time).ticks());
}

void setpic_timer::restart()
{
    _start_time=boost::posix_time::microsec_clock::local_time();
}


static unsigned long long int   _S_g_tick_status(0);
static std::vector<long>        _S_g_vkey_list;
static int                      _S_g_shm_count(0);

static cv::Rect                 _S_g_left_rect;
static cv::Rect                 _S_g_middle_rect;
static cv::Rect                 _S_g_right_rect;
static bool                     _S_g_b_save_rect_image(false);

static bool                     _S_g_b_save_image(false);
static image_type               _S_g_image_type(normal);
static int                      _S_g_image_param(0);
static int                      _S_g_board_index(0);

void reset_board_index()
{
    _S_g_board_index=0;
}

void set_save_image(image_type type,int param)
{
    switch(type)
    {
        case calibrate_board:
        {
            _S_g_image_type=calibrate_board;
            _S_g_b_save_image=true;
            std::fprintf(stderr, "%s\n","case : calibrate_board");
        }break;
        case correct:
        {
            switch(param)
            {
                case 0:
                {
                    _S_g_image_type=correct;
                    _S_g_image_param=0;
                    _S_g_b_save_image=true;
                }break;
                case 1:
                {
                    _S_g_image_type=correct;
                    _S_g_image_param=1;
                    _S_g_b_save_image=true;
                }break;
                default:
                {
                    ;
                }break;
            }
        }break;
        case normal:
        default:
        {
            std::fprintf(stderr, "%s\n","case : normal");
            ;
        }break;
    }
}


void set_shm(std::vector<long>& input_shm_key,int nCount)
{
    _S_g_vkey_list.clear();
    _S_g_vkey_list.reserve(input_shm_key.size());
    for(std::vector<long>::const_iterator _iter(input_shm_key.begin());_iter!=input_shm_key.end();++_iter)
    {
        _S_g_vkey_list.push_back(*_iter);
    }
    _S_g_shm_count=((nCount>=2)?((nCount<=16)?nCount:16):2);
}

void set_rect(int whichRect,int32_t x,int32_t y,int32_t x2,int32_t y2)
{
    switch(whichRect)
    {
        case 0:
        {
            _S_g_left_rect=cv::Rect(cv::Point(x,y),cv::Point(x2,y2));
        }break;
        case 2:
        {
            _S_g_right_rect=cv::Rect(cv::Point(x,y),cv::Point(x2,y2));
        }break;
        case 1:
        default:
        {
            _S_g_middle_rect=cv::Rect(cv::Point(x,y),cv::Point(x2,y2));
        }break;
    }
    _S_g_b_save_rect_image=true;
}


int callback_func(EBUS_IMAGE* img)
{
    cv::Mat _img=cv::Mat(img->Height,img->Width,CV_MAKETYPE(img->Depth,img->Channel),img->pImageBuf);
    cv::Mat src=_img;
    int32_t trigger_mode(-1);
    if(1!=trigger_mode)
    {
        //modbus client
    }

    if(_S_g_b_save_image)
    {
        switch(_S_g_image_type)
        {
            case calibrate_board:
            {

                char szfilename[128];
                std::snprintf(szfilename,sizeof(szfilename),"./runtime/data/board/%d.bmp",_S_g_board_index++%5);

                if(!boost::filesystem::exists(boost::filesystem::path("./runtime/data/board")))
                {
                    boost::filesystem::create_directories(boost::filesystem::path("./runtime/data/board"));
                }
                cv::imwrite(std::string(szfilename).c_str(),src);
                std::fprintf(stderr, "%s\n","save : calibrate_board");
            }break;
            case correct:
            {
                if(0==_S_g_image_param)
                {
                    char szfilename[128];
                    //std::memset(szfilename,0x00,sizeof(szfilename));
                    std::snprintf(szfilename,sizeof(szfilename),"./runtime/data/%s.bmp","black");
                    cv::imwrite(std::string(szfilename).c_str(),src);
                }
                else if(1==_S_g_image_param) // white
                {
                    char szfilename[128];
                    std::snprintf(szfilename,sizeof(szfilename),"./runtime/data/%s.bmp","white");
                    cv::imwrite(std::string(szfilename).c_str(),src);
                }
                else
                {
                    ;
                }
            }break;
            case normal:
            {

            }break;
        }
    }

    _S_g_b_save_image=false;
    _S_g_image_param=0;

    _S_g_tick_status+=1;
    int _shm_count(0);
    shm_st* share_handle(0);
    std::vector<shm_st*> shm_list;
    int m_shm_count=2;
    shm_list.reserve(_S_g_shm_count);
    while(_shm_count<_S_g_shm_count)
    {
        int shm_id=shmget(_S_g_vkey_list.at(_shm_count),sizeof(shm_st),0600|IPC_CREAT);
        if(shm_id<0)
        {
            perror("shmget");
        }
        else
        {
            shm_st* pshm=(shm_st*)shmat(shm_id,NULL,0);
            if(!pshm)
            {
                perror("shmat");
            }
            else
            {
                shmctl(shm_id,SHM_LOCK,0);
                shm_list.push_back(pshm);
            }
        }
        ++_shm_count; //if error all
    }
    static unsigned long long int _index(0);
    ++_index;
    share_handle=shm_list.at(_index%_S_g_shm_count);

    if(share_handle)
    {
        //static bool switcher_filter(true);
        static bool switcher_bright(false);
        static bool switcher_color_correct(false);
        static bool switcher_space_correct(false);
        static int n_save_buffer_max_count(0);
        static int n_save_image_count(0);
        static int image_flip_mode(-1);

        {
            lua_script lconfig("./camera.luac");

            if(0!=lconfig.get<bool>("switcher_color_correct",&switcher_color_correct))
            {
                switcher_color_correct=false;
            }

            if(0!=lconfig.get<bool>("switcher_space_correct",&switcher_space_correct))
            {
                switcher_space_correct=false;
            }

            if(0!=lconfig.get<int>("linescan_image_flip_mode",&image_flip_mode))
            {
                image_flip_mode=-1;
            }

            if(0!=lconfig.get<int>("linescan_save_original_max_count",&n_save_buffer_max_count))
            {
                n_save_buffer_max_count=0;
            }

            if(0!=lconfig.get<bool>("switcher_bright_equal",&switcher_bright))
            {
                switcher_bright=false;
            }

        }



        cv::Mat image=_img;
        share_handle->ExtendIntA=LOCK_WRITING;
        share_handle->nRWState=LOCK_WRITING;
        share_handle->nTotal                =0;
        share_handle->nState                =_S_g_tick_status;
        share_handle->nCmdState             =0;
        share_handle->nClassID              =0;
        share_handle->nGrabTime             =(int)(time_t)time(0);
        share_handle->LeftRect              =_S_g_left_rect;
        share_handle->MiddleRect            =_S_g_middle_rect;
        share_handle->RightRect             =_S_g_right_rect;
        share_handle->DataCorrct            =0;//nCaliCorrct;
        share_handle->DataInit              =0;//nCoInit;
        share_handle->TriggerMode           =trigger_mode;
        share_handle->uPixelType            =0;//BufferInfo.iPixelType;
        share_handle->uSizeX                =image.cols;
        share_handle->uSizeY                =image.rows;
        share_handle->uImageSize            =image.rows*image.step;
        share_handle->uMissingPackets       =0;//BufferInfo.iMissingPackets;
        share_handle->uAnnouncedBuffers     =0;//BufferInfo.iAnnouncedBuffers;
        share_handle->uQueuedBuffers        =0;//BufferInfo.iQueuedBuffers;
        share_handle->uOffsetX              =0;//BufferInfo.iOffsetX;
        share_handle->uOffsetY              =0;//BufferInfo.iOffsetY;
        share_handle->uAwaitDelivery        =0;//BufferInfo.iAwaitDelivery;
        share_handle->uBlockID              =0;//BufferInfo.iBlockId;
        share_handle->ulldTimeStamp         =img->Timestamp;// BufferInfo.iTimeStamp;
        //std::memcpy(share_handle->szImageData,BufferInfo.pImageBuffer,BufferInfo.iImageSize);
        std::memcpy(share_handle->szImageDataSrc,src.data,src.rows*src.step);

        if(switcher_space_correct)
        {
            static cv::Mat ImW;
            static bool b_read_ImW(true);
            static cv::Mat dark;
            static bool b_read_dark(true);
            if(b_read_ImW && boost::filesystem::exists(boost::filesystem::path("./runtime/data/white.bmp")))
            {
                ImW=cv::imread("./runtime/data/white.bmp");
                cv::flip(ImW,ImW,image_flip_mode);
                b_read_ImW=false;
            }
            if(b_read_dark && boost::filesystem::exists(boost::filesystem::path("./runtime/data/black.bmp")))
            {
                dark=cv::imread("./runtime/data/black.bmp");
                cv::flip(ImW,ImW,image_flip_mode);
                b_read_dark=false;
            }
            if(!src.empty() && !ImW.empty() && !dark.empty())
            {

                std::fprintf(stderr, "Begin %s\n","SpaceCorrect");
                static cv::Scalar dl;
                static cv::Scalar gain;
                static cv::Mat Im_weight(1, ImW.cols, CV_32FC1);
                SpaceCorrect(src,src,ImW,dark,Im_weight,dl,gain,0);
                std::fprintf(stderr, "End %s\n","SpaceCorrect");
                cv::imwrite("SpaceCorrect.jpg",src);
            }
        }

        std::memcpy(share_handle->szImageData,image.data,image.rows*image.step);
        share_handle->ExtendIntA=SAFT_READING;
        share_handle->nRWState=SAFT_READING;
    }

    for(std::vector<shm_st*>::const_iterator _iter(shm_list.begin());_iter!=shm_list.end();++_iter)
    {
        shmdt((const void*)*_iter);
    }

    if(_S_g_b_save_rect_image)
    {

    }

    return 0;
}