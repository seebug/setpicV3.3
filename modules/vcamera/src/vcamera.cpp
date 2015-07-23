#include "vcamera.h"
#include "callback.h"

#include <cstring>
#include <string>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <opencv2/opencv.hpp>




static image_mode_st param;
static int _current_camera_index(0);
static float __r(1.0f);
static float __g(1.0f);
static float __b(1.0f);
std::list<boost::filesystem::path>  lstTrain;
std::list<boost::filesystem::path>  lstRun;
std::list<std::string>              lstRunReal;


int _load_config(void* config)
{
    param=*((image_mode_st*)(uintptr_t)config);
    if(param.section<2)
    {
        std::fprintf(stderr, "[** NOTICE **] --section=%d %s\n",param.section,",but shared memory at least : 2 . auto changed ,--section=2");
        param.section=2;
    }
    std::fprintf(stderr, "[** NOTICE **] --xpressive=%s %s %s\n",param.xpressive.c_str(),",read image like :",param.xpressive.c_str());
    if(!boost::filesystem::exists(boost::filesystem::path(param.train.c_str())))
    {
        std::fprintf(stderr, "[** NOTICE **] --train=%s %s\n",param.train.c_str(),"is not exists . auto dircard .");
    }
    else
    {
        if(boost::filesystem::is_directory(boost::filesystem::path(param.train.c_str())))
        {
            bool train_shoud_be_a_config_file(false);
            std::fprintf(stderr, "[** NOTICE **] --train=%s\n",param.train.c_str()," should be a config file . but it's a directory your give.");
            assert(train_shoud_be_a_config_file);
        }
        else
        {
            //read config file
            std::fprintf(stderr, "[** NOTICE **] reading %s \n",param.train.c_str());
            std::ifstream fin(param.train.c_str(),std::ios::in);
            char szlinebuffer[1204*2];std::memset(szlinebuffer,0x00,sizeof(szlinebuffer));
            std::list<std::string> lstContainer;
            static uint32_t nLine(0);
            while(fin.getline(szlinebuffer,sizeof(szlinebuffer)))
            {
                std::string strLine;
                std::stringstream word(szlinebuffer);
                word>>strLine;
                lstContainer.push_back(strLine);
                ++nLine;
                std::fprintf(stderr, "[** NOTICE **] reading train : %d line   \r",nLine);
            }
            std::fprintf(stderr, "\n");
            fin.clear();
            fin.close();
            lstTrain.clear();
            if(0!=nLine)
            {
            //ANALYSIS AS CSV
                uint32_t pre(0);
                for(std::list<std::string>::iterator lineIter(lstContainer.begin());lineIter!=lstContainer.end();++lineIter)
                {

                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(*lineIter,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos) lstTrain.push_back(boost::filesystem::path(*pos));
                    ++pre;
                    std::fprintf(stderr, "[** NOTICE **] making train list %.lf precent      \r",((float)pre/nLine)*100);
                }
                std::fprintf(stderr, "\n");
            }
            else
            {
                std::fprintf(stderr, "[** NOTICE **] %s\n","no train image \n");
            }
            nLine=0;
        }
    }
    boost::filesystem::path _path_run(param.run.c_str());
    if(boost::filesystem::exists(_path_run))
    {
        if(boost::filesystem::is_directory(_path_run))
        {
            std::fprintf(stderr, "[** NOTICE **] --run=%s %s\n",param.run.c_str()," is directory.");
            static boost::xpressive::sregex_compiler rc;

            std::string strXpressive(param.xpressive);
            if(!rc[param.xpressive].regex_id())
            {
                rc[param.xpressive]=rc.compile(std::string(boost::replace_all_copy(boost::replace_all_copy(strXpressive,".","\\."),"*",".*")));
            }
            boost::filesystem::recursive_directory_iterator rd_iterator_end;
            lstRun.clear();
            for(boost::filesystem::recursive_directory_iterator pos(_path_run);pos!=rd_iterator_end;++pos)
            {
                if(!boost::filesystem::is_directory(*pos) && boost::xpressive::regex_match(pos->path().filename().string(),rc[param.xpressive]))
                {
                    lstRun.push_back(pos->path());
                    //std::fprintf(stderr, ");
                }
            }
            std::fprintf(stderr, "[** NOTICE **] read run directory :%d \n",lstRun.size());
        }
        else
        {
            std::fprintf(stderr, "[** NOTICE **] --run=%s %s\n",param.run.c_str(),"is not directory.");
            //read file
            std::ifstream fin(param.run.c_str(),std::ios::in);
            char szlinebuffer[1204*2];std::memset(szlinebuffer,0x00,sizeof(szlinebuffer));
            std::list<std::string> lstContainer;
            static uint32_t nLine(0);
            while(fin.getline(szlinebuffer,sizeof(szlinebuffer)))
            {
                std::string strLine;
                std::stringstream word(szlinebuffer);
                word>>strLine;
                lstContainer.push_back(strLine);
                ++nLine;
                std::fprintf(stderr, "[** NOTICE **] reading train : %d line   \r",nLine);
            }
            std::fprintf(stderr, "\n");
            fin.clear();
            fin.close();
            lstRun.clear();
            if(0!=nLine)
            {
            //ANALYSIS AS CSV
                uint32_t pre(0);
                for(std::list<std::string>::iterator lineIter(lstContainer.begin());lineIter!=lstContainer.end();++lineIter)
                {

                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(*lineIter,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos) lstRun.push_back(boost::filesystem::path(*pos));
                    ++pre;
                    std::fprintf(stderr, "[** NOTICE **] making run list %.f precent      \r",((float)pre/nLine)*100);
                }
                std::fprintf(stderr, "\n");
            }
            else
            {
                std::fprintf(stderr, "[** NOTICE **] %s\n","no run image \n");
            }
            nLine=0;
        }
    }
    else
    {
        std::fprintf(stderr, "[** NOTICE **] --run=%s %s\n",param.run.c_str(),"is not exists !");
        bool image_dir_or_config_file_is_not_exists(false);
        assert(image_dir_or_config_file_is_not_exists);
    }

    if(!lstTrain.empty() || !lstRun.empty())
    {
        //std::fprintf(stderr, "%d\n",__LINE__);
        //std::fprintf(stderr, "%s\n","making class map ");
        int nTotal(lstRun.size());
        int nCur(0);
        if(!lstRun.empty()) //nTotal != 0
        {
            std::map<std::string,int> image_parent_to_map;
            std::map<int, std::list<std::string> > map_to_image;
            int _index(0);
            for(std::list<boost::filesystem::path>::const_iterator _iter(lstRun.begin());_iter!=lstRun.end();++_iter)
            {
                //std::fprintf(stderr, "||||||||||||||%s\n",_iter->string().c_str());
                int _valid_index(-1);
                if(image_parent_to_map.find(_iter->parent_path().string())==image_parent_to_map.end())
                {
                    image_parent_to_map.insert(std::map<std::string,int>::value_type(_iter->parent_path().string(),_index++));
                    _valid_index=image_parent_to_map[_iter->parent_path().string()];
                }
                else
                {
                    _valid_index=image_parent_to_map[_iter->parent_path().string()];
                }

                if(map_to_image.find(_valid_index)==map_to_image.end())
                {
                    std::list<std::string> lstImage;
                    lstImage.push_back(_iter->string());
                    map_to_image.insert(std::map<int,std::list<std::string> >::value_type(_valid_index,lstImage));
                }
                else
                {
                    //std::fprintf(stderr, "%d\n",__LINE__);
                    if(param.count)
                    {
                        //std::fprintf(stderr, "%d\n",__LINE__);
                        if(map_to_image[_valid_index].size()<param.count)
                        {
                            map_to_image[_valid_index].push_back(_iter->string());
                            lstRunReal.push_back(_iter->string());
                        }
                    }
                    else
                    {
                        //std::fprintf(stderr, "%d\n",__LINE__);
                        map_to_image[_valid_index].push_back(_iter->string());
                        lstRunReal.push_back(_iter->string());
                        //std::fprintf(stderr, "=====%s\n",_iter->string().c_str());
                    }
                }

                ++nCur;
                std::fprintf(stderr, "[** NOTICE **] making image map : %.f precent       \r",(((float)nCur)/nTotal)*100);

            }
            std::fprintf(stderr, "\n");
        }
        else
        {
            std::fprintf(stderr, "[** NOTICE **] run empty\n");
        }

    }
    if(lstTrain.empty() || lstRunReal.empty()) return -1;
    return 0;
}



int _unload_config()
{
    if(!lstTrain.empty()) lstTrain.clear();
    if(!lstRunReal.empty()) lstRunReal.clear();
    return 0;
}

int _restore_config()
{
    return 0;
}

int _open()
{
    return 0;
}

int _close()
{
    return stop();
}

int _set_buffer()
{
    return 0;
}

int _unset_buffer()
{
    return 0;
}

int _set_current(int index)
{
    _current_camera_index=index;
    return 0;
}

int _get_current()
{
    return _current_camera_index;
}

int _set_whitebalance(float r,float g,float b)
{
    __r=r;__g=g;__b=b;
    return 0;
}

int _get_whitebalance(float& r,float& g,float& b)
{
    if(0!=&r) r=__r;
    if(0!=&g) g=__g;
    if(0!=&b) b=__b;
    return 0;
}

int _set_callback(eBusCameraCallBackFunction FUNC)
{
    return 0;
}

int _unset_callback()
{
    return 0;
}

int _set_trigger_free(void)
{
    return -1;
}

int _set_trigget_hardware(void)
{
    return -1;
}
//int get_trigger_mode(void);

int _set_width(long int nWidth)
{
    return 0;
}

int _get_width(long int& nHeight)
{
    return 0;
}

int _set_height(long int nHeight)
{
    return 0;
}

int _get_height(long int& nHeight)
{
    return 0;
}

int _set_exposure_abs(long int rexp,long int gexp,long int bexp)
{
    return 0;
}

int _get_exposure_abs(long int& rexp,long int& gexp,long int& bexp)
{
    return 0;
}

int _grab()
{
    int nTime(param.times);
    unsigned long long int ts(0);
    while(nTime)
    {
        for(std::list<std::string>::const_iterator con_iter(lstRunReal.begin());con_iter!=lstRunReal.end();++con_iter)
        {
            if(boost::filesystem::exists(boost::filesystem::path(*con_iter)))
            {
                cv::Mat img=cv::imread(std::string(*con_iter).c_str());
                EBUS_IMAGE* _pimg=new(std::nothrow) EBUS_IMAGE();
                if(_pimg)
                {
                    _pimg->Width    =img.cols;
                    _pimg->Height   =img.rows;
                    _pimg->Depth    =img.depth();
                    _pimg->Channel  =img.channels();
                    _pimg->pImageBuf=(void*)img.data;
                    _pimg->ImageSize=img.rows*img.step;
                    _pimg->Timestamp=ts++;
                    callback_func(_pimg);
                    delete _pimg;
                    _pimg=0;
                }
            }
        }
    }
    return 0;
}

int _stop()
{
    return 0;
}
