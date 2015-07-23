#include "getpic.h"
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

#include <unistd.h>
#include <sys/shm.h>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/time_duration.hpp>

//#include "core.h"
//#include "shmstruct.h"
//#define _SETPIC_IPC_FLAG_ 0x205

#include "global.h"
#include "lscript.h"

static std::vector<long> vShmKey;
static std::vector<shm_st*> vShared;
static shm_st* global_vcamgetpic_current_shm(NULL);

#ifndef NO_DEBUG_MESG
#define DEBUG_MSG(X) std::fprintf(stderr, "%s <line> %d <file> %s\n",X,__LINE__,__FILE__)
#else
#define DEBUG_MSG(X)
#endif

bool init_getpic(const char* rt_path)
{
    //using namespace xsetpic;
    if(!rt_path || 0==&vShmKey)
    {
        std::fprintf(stderr, "****init_getpic error :%s\n","invalid parameter !");
        return false;
    }

    if(!AutoMove(boost::filesystem::exists(AutoMove(boost::filesystem::path(rt_path)))))
    {
        std::fprintf(stderr, "****init_getpic error :%s\n","invalid runtime path,not exists !");
        return false;
    }
    else
    {
        if(!AutoMove(boost::filesystem::is_directory(AutoMove(boost::filesystem::path(rt_path)))))
        {
            std::fprintf(stderr, "****init_getpic error :%s\n","invalid runtime path,is not a directory !");
            return false;
        }
    }

    boost::filesystem::path tmp_path(rt_path);
    boost::filesystem::path tmp_path2(boost::filesystem::system_complete(tmp_path));;
    //std::string str_strart_up_dir(strart_up_dir.string());
    boost::filesystem::path _rt_path(tmp_path2);
    char vir_dir_sub[256];
    std::memset(vir_dir_sub,0x00,sizeof(vir_dir_sub));
    std::sprintf(vir_dir_sub,"%s/%s",_rt_path.string().c_str(),"map");
    vir_dir_sub[sizeof(vir_dir_sub)-1]='\0';
    boost::filesystem::path dir_map(std::string(vir_dir_sub).c_str());
    if(!boost::filesystem::exists(dir_map))
    {
        boost::filesystem::create_directories(dir_map);
    }
    else
    {
        if(!boost::filesystem::is_directory(dir_map))
        {
            boost::filesystem::remove(dir_map);
            boost::filesystem::create_directories(dir_map);
        }
    }


    std::list<boost::filesystem::path> list_shm_lua_file_path;
    list_shm_lua_file_path.clear();
    int32_t n_max_shared_memory_count(16);
    vShmKey.clear();
    vShmKey.reserve(16);
    while(n_max_shared_memory_count>0)
    {
        char _sz_file_path[256];
        std::memset(_sz_file_path,0x00,sizeof(_sz_file_path));
        std::sprintf(_sz_file_path,"%s/%d.lua",dir_map.string().c_str(),n_max_shared_memory_count-1);
        const boost::filesystem::path shminfo_path(std::string(_sz_file_path).c_str());
lable_recheck_shminfo_file_flag:
        if(boost::filesystem::exists(shminfo_path))
        {
            if(!boost::filesystem::is_directory(shminfo_path))
            {
                //bool b_over_write(false);
                uint32_t set_pic_base(_SETPIC_IPC_FLAG_);
                key_t real_key(ftok(shminfo_path.string().c_str(),set_pic_base));
                lua_script shm_info_script(shminfo_path.string().c_str());
                std::string _shm_file_path(".");
                //key_t shm_info_id(0);
                long shm_info_id(0);
                shm_info_script.get<std::string>("path",&_shm_file_path);
                shm_info_script.get<long>("id",&shm_info_id);

                if(0!=std::strcmp(_shm_file_path.c_str(),shminfo_path.string().c_str()) ||
                    real_key!=shm_info_id)
                {
                    std::fstream _file_shm_info(shminfo_path.c_str(),std::ios::out);
                    if(_file_shm_info)
                    {
                        try
                        {
                            std::string _shm_info_open_app( "#!/usr/bin/env lua\n");
                            _file_shm_info<<_shm_info_open_app<<std::endl;
                            std::string _shm_info_open_tip( "--[[\n\n"
                                                            "    ATTENTION : THIS FILE IS AUTO CREATED BY CAMERA SERVICE SETPIC V3 ,USING FOR SHARED MEMORY.\n"
                                                            "DO NOT CHANGE OR MOVE IT . ANY CHANGED IN THIS FILE MAY MAKE SHARED MEMORY NOT WORK .\n\n"
                                                            "                                           wangbin <wangbin@inovance.cn>\n"
                                                            "]]--\n");
                            _file_shm_info<<_shm_info_open_tip<<std::endl;
                            _file_shm_info<<"path   =\""<<shminfo_path.string()<<"\""<<std::endl;
                            _file_shm_info<<"id     ="<<real_key;
                            char _shm_info_delete_function[512];
                            std::memset(_shm_info_delete_function,0x00,sizeof(_shm_info_delete_function));
                            std::sprintf(_shm_info_delete_function,"\n\n--delete command: ipcrm -m shmid \n"
                                                               "function delete_this_shared_memory(...)\n"
                                                               "    io.write(tostring(_VERSION) .. tostring(\"\\t\") .. tostring(os.date()) .. tostring(\"\\n\"))\n"
                                                               "    if os.getenv [[USER]] ~= tostring(\"root\") then\n"
                                                               "        io.write(tostring(\"For Better Execute,Suggest You Login As \\\"root\\\"\\n\"))\n"
                                                               "    end\n"
                                                               "    os.execute(\"ipcrm -m %ld\")\n"
                                                               "end\n"
                                                                    ,real_key);
                            _shm_info_delete_function[sizeof(_shm_info_delete_function)-1]='\0';
                            _file_shm_info<<std::string(_shm_info_delete_function)<<std::endl;
                        }
                        catch(std::exception& e)
                        {
                            std::fprintf(stderr, "%s\n",e.what());

                        }
                        _file_shm_info.close();
                    }
                }
                else
                {
                    vShmKey.push_back(real_key);
                    --n_max_shared_memory_count;
                }

            }
            else // exist ,but is dir
            {
                boost::filesystem::remove_all(shminfo_path);//recursive delete dir
                std::fstream _file_shm_info(shminfo_path.string().c_str(),std::ios::out);
                if(_file_shm_info)
                {
                    _file_shm_info.close();
                }
                goto lable_recheck_shminfo_file_flag;
            }
        }
        else //not exists
        {
            std::fstream _file_shm_info(shminfo_path.string().c_str(),std::ios::out);
            if(_file_shm_info)
            {
                _file_shm_info.close();
            }
            goto lable_recheck_shminfo_file_flag;
        }
    }
    std::reverse(vShmKey.begin(),vShmKey.end());
    for(std::vector<long>::const_iterator _iter(vShmKey.begin());_iter!=vShmKey.end();++_iter)
    {
        std::fprintf(stderr, "%ld\n",*_iter);
    }
    return true;
}




bool init_shared_memory(const int count)
{
    const int _count((count>=2)?((count<=16)?count:16):2);
    if(vShmKey.empty())
    {
        std::fprintf(stderr, "%s\n","not shared memory key getted !");
        return false;
    }
    int _index(0);
    vShared.clear();
    vShared.reserve(_count);
    while(_index<_count)
    {
        int shm_id=shmget(vShmKey.at(_index),sizeof(shm_st),0600|IPC_CREAT);
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
                vShared.push_back(pshm);
            }
        }
        ++_index; //if error all
    }
    if(_count==vShared.size()) return true;
    else return false;
}

bool get_min_nState(uint64_t p1,uint64_t p2)
{
    return (bool)(p1<=p2);
}


void get_read_part()
{
    //std::fprintf(stderr, "vShared size = %d\n",vShared.size() );
    std::vector<shm_st*> _n_status;
    _n_status.reserve(vShared.size());
    for(std::vector<shm_st*>::const_iterator _iter(vShared.begin());_iter!=vShared.end();++_iter)
    {
        if((*_iter)->nRWState==(unsigned int)SAFT_READING)
        {
            _n_status.push_back(*_iter);
        }
    }

    if(0==_n_status.size())
    {
        global_vcamgetpic_current_shm=0;
    }
    else if(1==_n_status.size())
    {
        global_vcamgetpic_current_shm=_n_status.at(0);
    }
    else
    {

        //std::sort(_n_status.begin(),_n_status.end(),get_min_shm_st_nState);

        std::vector<uint64_t> vState;
        vState.reserve(_n_status.size());
        for(uint32_t i(0);i<_n_status.size();++i)
        {
            vState.push_back(_n_status[i]->nState);
        }
        std::vector<uint64_t> vState_(vState.begin(),vState.end());
        std::sort(vState.begin(),vState.end(),get_min_nState);
        uint32_t index(-1);
        for (uint32_t i(0); i < vState.size(); ++i)
        {
            if(vState[0]==vState_[i])
            {
                index=i;
                break;
            }
        }
        if(-1!=index)
        {
            global_vcamgetpic_current_shm=_n_status.at(index);
        }
        else
        {
            global_vcamgetpic_current_shm=0;
        }
    }

    //return 0;
}

bool get_image(cv::Mat& output_src,cv::Mat& output, int& total,int& classid,int& grab_time,unsigned long long int& ulldTimeStamp,cv::Rect* outLeft,cv::Rect* outMiddle,cv::Rect* outRight,int& nDataCorrct,int& nDataInit,int& nTriggerMode)
{

    if(0==&output && 0==&output_src) return false;
    get_read_part();
    if(!global_vcamgetpic_current_shm) return false;
    shm_st* _pshm=global_vcamgetpic_current_shm;
    _pshm->nRWState=LOCK_READING;
    if(_pshm->uSizeX <=0 || _pshm->uSizeY <=0 || _pshm-> uImageSize <=0 || _pshm->uImageSize < (_pshm->uSizeX*_pshm->uSizeY))
    {
        _pshm->nRWState=SAFT_DELETED;
        return false;
    }
    cv::Mat _img,_img_src;
    char ch((char)(_pshm->uImageSize/(_pshm->uSizeX*_pshm->uSizeY)));
    //std::fprintf(stderr, "channel = %d\n",ch);
    _img.create(_pshm->uSizeY,_pshm->uSizeX,CV_MAKETYPE(CV_8U,ch));
    _img_src.create(_pshm->uSizeY,_pshm->uSizeX,CV_MAKETYPE(CV_8U,ch));
    std::memcpy(_img.data,_pshm->szImageData,_img.rows*_img.step);
    std::memcpy(_img_src.data,_pshm->szImageDataSrc,_img_src.rows*_img_src.step);
    total                               =_pshm->nTotal;                                 //total
    //int width                           =_pshm->uSizeX;                                 //width
    //int height                          =_pshm->uSizeY;                                //height
    //int ImageSize                       =_pshm->uImageSize;                         //size
    classid                             =_pshm->nClassID;                            //classid
    grab_time                           =_pshm->nGrabTime;
    ulldTimeStamp                       =_pshm->ulldTimeStamp;
    nDataCorrct                         =_pshm->DataCorrct;
    nDataInit                           =_pshm->DataInit;
    if(outLeft)
    {
        *outLeft= _pshm->LeftRect;
    }
    if(outMiddle)
    {
        *outMiddle= _pshm->MiddleRect;
    }
    if(outRight)
    {
        *outRight= _pshm->RightRect;
    }
    nTriggerMode=_pshm->TriggerMode;
    _pshm->nRWState=SAFT_DELETED;

    //cv::imwrite("original.jpg",_img);
    if(!_img.empty() && !_img_src.empty())
    {
        _img.copyTo(output);
        _img_src.copyTo(output_src);
        _img.release();
        _img_src.release();
        return true;
    }
    else
    {
        return false;
    }
}

bool get_image_test(cv::Mat& output_src,unsigned long long int& ulldTimeStamp)
{
    if(0==&output_src) return false;
    //get_read_part();

    shm_st* temp_vcamgetpic_current_shm(NULL);

    std::vector<shm_st*> _n_status;
    _n_status.reserve(vShared.size());
    for(std::vector<shm_st*>::const_iterator _iter(vShared.begin());_iter!=vShared.end();++_iter)
    {
        if((*_iter)->nRWState==(unsigned int)SAFT_READING)
        {
            _n_status.push_back(*_iter);
        }
    }

    if(0==_n_status.size())
    {
        temp_vcamgetpic_current_shm=0;
    }
    else if(1==_n_status.size())
    {
        temp_vcamgetpic_current_shm=_n_status.at(0);
    }
    else
    {

        //std::sort(_n_status.begin(),_n_status.end(),get_min_shm_st_nState);

        std::vector<uint64_t> vState;
        vState.reserve(_n_status.size());
        for(uint32_t i(0);i<_n_status.size();++i)
        {
            vState.push_back(_n_status[i]->nState);
        }
        std::vector<uint64_t> vState_(vState.begin(),vState.end());
        std::sort(vState.begin(),vState.end(),get_min_nState);
        uint32_t index(-1);
        for (uint32_t i(0); i < vState.size(); ++i)
        {
            if(vState[0]==vState_[i])
            {
                index=i;
                break;
            }
        }
        if(-1!=index)
        {
            temp_vcamgetpic_current_shm=_n_status.at(index);
        }
        else
        {
            temp_vcamgetpic_current_shm=0;
        }
    }



    if(!temp_vcamgetpic_current_shm) return false;
    shm_st* _pshm=temp_vcamgetpic_current_shm;
    if(_pshm->uSizeX <=0 || _pshm->uSizeY <=0 || _pshm-> uImageSize <=0 || _pshm->uImageSize < (_pshm->uSizeX*_pshm->uSizeY))
    {
        return false;
    }
    if(_pshm->ExtendIntA==SAFT_DELETED) return false;
    _pshm->ExtendIntA=LOCK_READING;
    cv::Mat _img;
    char ch((char)(_pshm->uImageSize/(_pshm->uSizeX*_pshm->uSizeY)));
    _img.create(_pshm->uSizeY,_pshm->uSizeX,CV_MAKETYPE(CV_8U,ch));
    std::memcpy(_img.data,_pshm->szImageDataSrc,_img.rows*_img.step);
    ulldTimeStamp==_pshm->ulldTimeStamp;
    _pshm->ExtendIntA=SAFT_DELETED;
    if(_img.empty())
    {
        _img.copyTo(output_src);
        _img.release();
        return true;
    }
    else
    {
        return false;
    }
}

bool uninit_shared_memory()
{
    if(vShared.empty()) return true;
    for(std::vector<shm_st*>::const_iterator _iter(vShared.begin());_iter!=vShared.end();++_iter)
    {
        shmdt((const void*)*_iter);
    }
    vShared.empty();
    return true;
}

