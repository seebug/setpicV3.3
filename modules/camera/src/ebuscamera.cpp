#include "ebuscam.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <list>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <dlfcn.h>

#include <PvResult.h>
#include <PvSystem.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>
#include <PvConfigurationReader.h>
#include <PvConfigurationWriter.h>
#include <PvFilterRGB.h>
#include <PvBufferWriter.h>
#include <PvPixelType.h>

#include <opencv2/opencv.hpp>

#include "lscript.h"

static int nCurrent(0);

#define BUFFER_COUNT 8

struct ebus_camera_type
{
    bool                            valid_config;
    std::string                     strConfig;

    bool                            valid_wb;
    float                           wb_rval;
    float                           wb_gval;
    float                           wb_bval;

    float                           wb_offset_rval;
    float                           wb_offset_gval;
    float                           wb_offset_bval;

    eBusCameraCallBackFunction      cb_function;

//==============================================
    PvConfigurationReader           lReader;
    PvResult                        lReaderRet;         //read

    PvDevice                        lDevice;
    PvResult                        lDeviceRet;         //store
    //PvResult                        lDeviceOpenRet;

    PvStream                        lStream;
    PvResult                        lStreamRet;         //store
    PvResult                        lStreamOpenRet;     //open

    PvDeviceInfo                    *lDeviceInfo;
    PvResult                        lDeviceInfoRet;     //connect

    PvBuffer*                       lBuffers;
    PvUInt32                        lBufferCount;
    PvGenInteger                    *lTLLocked;
    PvFilterRGB                     lFilter;

    pthread_t                       hStreamThread;
    bool                            bRunThread;

};

static ebus_camera_type ebus_camera[4];

// -1 invalid parameter
// -2 file not exists
// -3 failed

int set_device_enum_value(const std::string& value,const char* key)
{
    int _nCurrent=get_current();
    if(ebus_camera[_nCurrent].lDevice.IsConnected())
    {
        PvGenParameterArray* lDeviceParams=ebus_camera[_nCurrent].lDevice.GetGenParameters();
        PvGenParameter *lGenParameter=lDeviceParams->Get(key);
        if(!lGenParameter->IsVisible( PvGenVisibilityGuru ) || !lGenParameter->IsAvailable())
        {
            std::fprintf( stderr,"[ebuscamera.so] %s Parameter Is Not Visible Or Available!\n", key );
            return -3;
        }

        PvGenEnum *lPvGenEnum=dynamic_cast<PvGenEnum *>( lGenParameter );

        PvString lValue(value.c_str());
        PvResult ret = lPvGenEnum->SetValue(lValue);
        if ( !ret.IsOK() )
        {
            std::fprintf( stderr, "[ebuscamera.so] Error set %s enum value to device! Error info:%s\n", key, ret.GetDescription().GetAscii() );
            return -3;
        }
        return 0;
    }
    else
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Not Connected.");
        return -3;
    }
}

int set_device_int_value(const long long int& value,const char* key)
{
    int _nCurrent=get_current();
    if(ebus_camera[_nCurrent].lDevice.IsConnected())
    {
        PvGenParameterArray* lDeviceParams=ebus_camera[_nCurrent].lDevice.GetGenParameters();
        PvGenParameter *lGenParameter=lDeviceParams->Get(key);
        if(!lGenParameter->IsVisible( PvGenVisibilityGuru ) || !lGenParameter->IsAvailable())
        {
            std::fprintf( stderr,"[ebuscamera.so] %s Parameter Is Not Visible Or Available!\n", key );
            return -3;
        }

        PvGenInteger* lPvGenInt = dynamic_cast<PvGenInteger*>( lGenParameter );

        PvInt64 lValue(value);
        PvResult ret = lPvGenInt->SetValue(lValue);
        if ( !ret.IsOK() )
        {
            std::fprintf( stderr, "[ebuscamera.so] Error set %s int value to device! Error info:%s\n", key, ret.GetDescription().GetAscii() );
            return -3;
        }
        return 0;
    }
    else
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Not Connected.");
        return -3;
    }
}

int get_device_int_value(long int& value,const char* lpszFeature)
{
    if(!&value) return -2;
    int _nCurrent=get_current();
    if(!ebus_camera[_nCurrent].lDevice.IsConnected()) return -3;
    PvGenParameterArray* lDeviceParams = ebus_camera[_nCurrent].lDevice.GetGenParameters();
    PvGenParameter* lGenParameter=lDeviceParams->Get(lpszFeature);
    if(!lGenParameter->IsVisible(PvGenVisibilityGuru) || !lGenParameter->IsAvailable())
    {
        std::fprintf(stderr, "[ebuscamera.so] %s Parameter is Not Visible or Available \n",lpszFeature);
        return -3;
    }
    PvGenInteger *lPvGenInt=static_cast<PvGenInteger*>(lGenParameter);
    PvInt64 lValue;
    PvResult ret=lPvGenInt->GetValue(lValue);
    if(!ret.IsOK())
    {
        std::fprintf(stderr, "[ebuscamera.so] Error get %s int value from Device! Error info: %s \n",lpszFeature,ret.GetDescription().GetAscii());
        return -3;
    }
    value=lValue;
    return 0;
}

int get_stream_int_value(long int& value,const char* lpszFeature)
{
    if(!&value) return -2;
    int _nCurrent=get_current();
    if(!ebus_camera[_nCurrent].lStream.IsOpen())
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Failed,Stream Is Not Open.");
        return -3;
    }

    PvGenParameterArray* lGenParameters=ebus_camera[_nCurrent].lStream.GetParameters();
    PvGenParameter *lGenParameter=lGenParameters->Get(lpszFeature);

    if(!lGenParameter->IsVisible(PvGenVisibilityGuru) || !lGenParameter->IsAvailable())
    {
        std::fprintf(stderr, "[ebuscamera.so] %s Parameter is Not Visible or Available \n",lpszFeature);
        return -3;
    }

    PvGenInteger *lPvGenInt=static_cast<PvGenInteger*>(lGenParameter);
    PvInt64 lValue;
    PvResult ret=lPvGenInt->GetValue(lValue);
    if(!ret.IsOK())
    {
        std::fprintf(stderr, "[ebuscamera.so] Error get %s int value from stream! Error info: %s \n",lpszFeature,ret.GetDescription().GetAscii());
    }   return -3;

    value=lValue;
    return 0;
}

int set_stream_int_value(const long long int value,const char* key)
{
    int _nCurrent=get_current();
    if(!ebus_camera[_nCurrent].lStream.IsOpen())
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Stream Is Not Open!");
        return -3;
    }
    PvGenParameterArray* lGenParameters=ebus_camera[_nCurrent].lStream.GetParameters();
    PvGenParameter *lGenParameter=lGenParameters->Get(key);

    if ( !lGenParameter->IsVisible( PvGenVisibilityGuru ) || !lGenParameter->IsAvailable() )
    {
        std::fprintf(stderr, "[ebuscamera.so] %s Parameter is not Visible or Available\n",key);
        return -3;
    }

    PvGenInteger* lPvGenInt=static_cast<PvGenInteger*>(lGenParameter);
    PvInt64 lValue=value;
    PvResult ret=lPvGenInt->SetValue(lValue);

    if ( !ret.IsOK() )
    {
        std::fprintf( stderr,"[ebuscamera.so] Error set %s int value to stream!Error info:%s\n", key, ret.GetDescription().GetAscii() );
        return -3;
    }
    return 0;
}

/*************************************************
*
* SCRIPT
*
\************************************************/

static lua_script *pscript;
static bool is_script_loaded(false);

int __load_config_target(const char* config)
{
    if(is_script_loaded && NULL!=pscript) return 0;
    if(NULL==config || 0==strcmp("",config)) return -2;
    pscript=new lua_script(config);
    if(pscript->valid()) return 0;
    else return -3;
}

int __unload_config_target(void)
{
    if(!is_script_loaded && NULL==pscript) return -1;
    if(!pscript)
    {
        delete pscript;
        pscript=NULL;
    }
    is_script_loaded=false;
    return -3;
}

int __auto_config_impl(const config_type config)
{
    switch(config.category)
    {
        case category_unknown:
        {
            ;
        }break;
        case category_device:
        {
            switch(config.type_name)
            {
                case unknown_key_word:
                {
                    ;
                }break;
                case string_key_word:
                {
                    return set_device_enum_value(config.string_value,config.key_name.c_str());

                }break;
                case integer_key_word:
                {
                    return set_device_int_value(config.int32_value,config.key_name.c_str());
                }break;
                case long_integer_key_word:
                {
                    return set_device_int_value(config.int64_value,config.key_name.c_str());
                }break;
                case double_key_word:
                {
                    ;// ?
                }break;
                case command_key_word:
                {
                    ;// ?
                }break;
                default:
                {
                    ;// ?
                }break;
            } //end switch
        }break;
        case category_stream:
        {
            switch(config.type_name)
            {
                case unknown_key_word:
                {
                    ;
                }break;
                case string_key_word:
                {
                    //set_stream_int_value
                }break;
                case integer_key_word:
                {
                    return set_stream_int_value(config.int32_value,config.key_name.c_str());
                }break;
                case long_integer_key_word:
                {
                    return set_stream_int_value(config.int64_value,config.key_name.c_str());
                }break;
                case double_key_word:
                {

                }break;
                case command_key_word:
                {

                }break;
                default:
                {

                }break;
            } // end switch

        }break;
    }

}

int auto_config(const char* table_name)
{
    //return 0;
    if(0!=__load_config_target("genicam.lua"))
    {
        return -1;
    }
    std::vector<config_type> config;
    config.reserve(20);
    if(0!=pscript->read_table_item(table_name,config)) return -2;
    #ifdef HAVE_CXX11_SUPPORT
    for_each(config.begin(),config.end(),
                                        [&](config_type config_value) mutable -> int
                                        {
                                            return __auto_config_impl(config_value);
                                        }
            );
    #else
    for_each(config.begin(),config.end(),__auto_config_impl);
    #endif
    return 0;
}



int load_config(const char* lpszConfig)
{
    if(!lpszConfig) return -1;
    if(0!=access(lpszConfig,0))
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","file not exists");
        return -2;
    }
    int _nCurrent(get_current());
    ebus_camera[_nCurrent].strConfig=std::string(lpszConfig);
    PvString lFilenameToSave(ebus_camera[_nCurrent].strConfig.c_str());
    ebus_camera[_nCurrent].lReaderRet=ebus_camera[_nCurrent].lReader.Load(lFilenameToSave);
    if(!ebus_camera[_nCurrent].lReaderRet.IsOK())
    {
        std::fprintf(stderr, "[ebuscamera.so] Failed To Load Camera Config ! %s\n",ebus_camera[_nCurrent].lReaderRet.GetDescription().GetAscii());
        return -3;
    }
    std::fprintf(stderr, "[ebuscamera.so] %s\n","Load Camera Config Success!");
    return 0;
}

int unload_config()
{
    stop();
    unset_buffer();
    close();
    int _nCurrent=get_current();
    if(ebus_camera[_nCurrent].lDevice.IsConnected())
    {
        ebus_camera[_nCurrent].lDevice.Disconnect();
        ebus_camera[_nCurrent].lDeviceRet=PvResult(PvResult::Code::STATE_ERROR);
    }
    return 0;
}

int restore_config(void)
{
    int _nCurrent(get_current());
    if(!ebus_camera[_nCurrent].lReaderRet.IsOK())
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Error : Is Config Load Success ? ");
        return -3;
    }
    ebus_camera[_nCurrent].lDeviceRet=ebus_camera[_nCurrent].lReader.Restore(0,&ebus_camera[_nCurrent].lDevice);
    if(ebus_camera[_nCurrent].lDeviceRet.IsOK())
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Restore Device Success .");
        auto_config("restore_config_set_device");
        ebus_camera[_nCurrent].lDevice.NegotiatePacketSize();
        ebus_camera[_nCurrent].lStreamRet=ebus_camera[_nCurrent].lReader.Restore(0,ebus_camera[_nCurrent].lStream);
        if(ebus_camera[_nCurrent].lStreamRet.IsOK())
        {
            std::fprintf(stderr, "[ebuscamera.so] %s\n","Restore Stream Success .");
            auto_config("restore_config_set_stream");
            PvString strRed("redgain"),strGreen("greengain"),strBlue("bluegain");
            PvString strRedVal("1.0"),strGreenVal("1.0"),strBlueVal("1.0");
            PvString strRedOffSet("redoffset"),strGreenOffSet("greenoffset"),strBlueOffSet("blueoffset");
            PvString strRedOffSetVal("0.0"),strGreenOffSetVal("0.0"),strBlueOffSetVal("0.0");
            PvResult rRet=ebus_camera[_nCurrent].lReader.Restore(strRed,strRedVal);
            PvResult gRet=ebus_camera[_nCurrent].lReader.Restore(strGreen,strGreenVal);
            PvResult bRet=ebus_camera[_nCurrent].lReader.Restore(strBlue,strBlueVal);
            PvResult rOffSetRet=ebus_camera[_nCurrent].lReader.Restore(strRedOffSet,strRedOffSetVal);
            PvResult gOffSetRet=ebus_camera[_nCurrent].lReader.Restore(strGreenOffSet,strGreenOffSetVal);
            PvResult bOffSetRet=ebus_camera[_nCurrent].lReader.Restore(strBlueOffSet,strBlueOffSetVal);
            if(rRet.IsOK() && gRet.IsOK() && bRet.IsOK())
            {
                ebus_camera[_nCurrent].wb_rval=strtof(strRedVal.GetAscii(),NULL);
                ebus_camera[_nCurrent].wb_gval=strtof(strGreenVal.GetAscii(),NULL);
                ebus_camera[_nCurrent].wb_bval=strtof(strBlueVal.GetAscii(),NULL);
                ebus_camera[_nCurrent].valid_wb=true;
                set_whitebalance(ebus_camera[_nCurrent].wb_rval,ebus_camera[_nCurrent].wb_gval,ebus_camera[_nCurrent].wb_bval);
            }

            if(rOffSetRet.IsOK() && gOffSetRet.IsOK() && bOffSetRet.IsOK())
            {
                ebus_camera[_nCurrent].wb_offset_rval=strtof(strRedOffSetVal.GetAscii(),NULL);
                ebus_camera[_nCurrent].wb_offset_gval=strtof(strGreenOffSetVal.GetAscii(),NULL);
                ebus_camera[_nCurrent].wb_offset_bval=strtof(strBlueOffSetVal.GetAscii(),NULL);
            }
            std::fprintf(stderr, "[ebuscamera.so] white balance:r= %lf\tg=%lf\tb=%lf\n"
                ,ebus_camera[_nCurrent].wb_rval
                ,ebus_camera[_nCurrent].wb_gval
                ,ebus_camera[_nCurrent].wb_bval);
            return 0;
        }
        else
        {
            std::fprintf(stderr, "[ebuscamera.so] %s\n","Failed to Restore Stream ...");
            return -3;
        }
    }
    else
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Cannot Not Connect With This Config ...");
        return -3;
    }

}


int set_current(int val)
{
    nCurrent=val;
    return 0;
}

int get_current(void)
{
    return nCurrent;
}

int open()
{
    int _nCurrent=get_current();
    if(ebus_camera[_nCurrent].lDeviceRet.IsOK() && ebus_camera[_nCurrent].lStreamRet.IsOK())
    {
        if(!ebus_camera[_nCurrent].lDevice.IsConnected())
        {
            ebus_camera[_nCurrent].lDeviceInfoRet=ebus_camera[_nCurrent].lDevice.Connect(ebus_camera[_nCurrent].lDeviceInfo);
            if(ebus_camera[_nCurrent].lDeviceInfoRet.IsOK())
            {
                std::fprintf(stderr, "[ebuscamera.so] %s\n","Device Is Connect Done !");
                ebus_camera[_nCurrent].lStreamOpenRet=ebus_camera[_nCurrent].lStream.Open(ebus_camera[_nCurrent].lDeviceInfo->GetMACAddress());
                if(ebus_camera[_nCurrent].lStreamOpenRet.IsOK())
                {
                    std::fprintf(stderr, "[ebuscamera.so] %s\n","Stream Is Open Done !");
                    return 0;
                }
                else
                {
                    std::fprintf(stderr, "[ebuscamera.so] %s\n",ebus_camera[_nCurrent].lStreamOpenRet.GetDescription().GetAscii());
                    std::fprintf(stderr, "[ebuscamera.so] %s\n","Failed,Cannot Open Stream .");
                    return -3;
                }
            }
            else
            {
                std::fprintf(stderr, "[ebuscamera.so] %s\n",ebus_camera[_nCurrent].lDeviceInfoRet.GetCodeString().GetAscii());
                std::fprintf(stderr, "[ebuscamera.so] %s\n","Failed , Device Is Not Connected !");
                return -3;
            }
        }
        else
        {
            //std::fprintf(stderr, "[ebuscamera.so] %s\n","Device Is Not Connected!");
            //device is connected
            //check stream
            if(ebus_camera[_nCurrent].lStream.IsOpen())
            {
                std::fprintf(stderr, "[ebuscamera.so] %s\n","Stream Is Open Done !");
                return 0;
            }
            else
            {
                ebus_camera[_nCurrent].lStreamOpenRet=ebus_camera[_nCurrent].lStream.Open(ebus_camera[_nCurrent].lDeviceInfo->GetMACAddress());
                if(ebus_camera[_nCurrent].lStreamOpenRet.IsOK())
                {
                    std::fprintf(stderr, "[ebuscamera.so] %s\n","Stream Is Open Done !");
                    return 0;
                }
                else
                {
                    std::fprintf(stderr, "[ebuscamera.so] %s\n",ebus_camera[_nCurrent].lStreamOpenRet.GetDescription().GetAscii());
                    std::fprintf(stderr, "[ebuscamera.so] %s\n","Failed,Cannot Open Stream .");
                    return -3;
                }
            }
        }
    }
    else
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Both Device and Stream Are Restore Success ?");
        return -3;
    }
}

int close()
{
    int _nCurrent=get_current();
    if(ebus_camera[_nCurrent].lStream.IsOpen())
    {
        //change by wangbin ,2015-01-27
        // when restore config,the stream can open automation,
        // unload config,auto close by itself

        //ebus_camera[_nCurrent].lStream.Close();
        //ebus_camera[_nCurrent].lStreamOpenRet=PvResult(PvResult::Code::STATE_ERROR);
    }
    return 0;
}

int set_buffer()
{
    int _nCurrent=get_current();
    if(!(ebus_camera[_nCurrent].lDeviceRet.IsOK() && ebus_camera[_nCurrent].lStreamRet.IsOK()))
    {
        std::fprintf(stderr, "[ebuscamera.so] %s\n","Both Device and Stream Are Restore Success ?");
        return -3;
    }
    PvInt64     lSize(0);
    PvGenParameterArray *lDeviceParams = ebus_camera[_nCurrent].lDevice.GetGenParameters();
    PvGenInteger *lPayloadSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "PayloadSize" ) );
    lPayloadSize->GetValue(lSize);
    ebus_camera[_nCurrent].lBufferCount=(ebus_camera[_nCurrent].lStream.GetQueuedBufferMaximum()< BUFFER_COUNT)?ebus_camera[_nCurrent].lStream.GetQueuedBufferMaximum():BUFFER_COUNT;
    ebus_camera[_nCurrent].lBuffers=new PvBuffer[ebus_camera[_nCurrent].lBufferCount];
    for(PvUInt32 i(0);i<ebus_camera[_nCurrent].lBufferCount;++i)
    {
        ebus_camera[_nCurrent].lBuffers[i].Alloc(static_cast<PvUInt32>(lSize));
    }
    ebus_camera[_nCurrent].lDevice.SetStreamDestination(ebus_camera[_nCurrent].lStream.GetLocalIPAddress(),ebus_camera[_nCurrent].lStream.GetLocalPort());
    for(PvUInt32 i(0);i<ebus_camera[_nCurrent].lBufferCount;++i)
    {
        ebus_camera[_nCurrent].lStream.QueueBuffer(ebus_camera[_nCurrent].lBuffers+i);
    }
    return 0;
}

int unset_buffer()
{
    int _nCurrent=get_current();
    if(ebus_camera[_nCurrent].lStreamOpenRet.IsOK())
    {
        ebus_camera[_nCurrent].lStream.AbortQueuedBuffers();
        while(ebus_camera[_nCurrent].lStream.GetQueuedBufferCount()>0)
        {
            PvBuffer *_lBuffer=0;
            PvResult _lOperationResult;
            ebus_camera[_nCurrent].lStream.RetrieveBuffer(&_lBuffer,&_lOperationResult);
        }

        if(ebus_camera[_nCurrent].lBuffers)
        {
            delete [] ebus_camera[_nCurrent].lBuffers;
            ebus_camera[_nCurrent].lBuffers=NULL;
            ebus_camera[_nCurrent].lBufferCount=0;
        }
    }

    return 0;
}


int set_whitebalance(float r,float g,float b)
{
    int _nCurrent=get_current();
    ebus_camera[_nCurrent].lFilter.SetGainR(r);
    ebus_camera[_nCurrent].lFilter.SetGainG(g);
    ebus_camera[_nCurrent].lFilter.SetGainB(b);
    return 0;
}

int get_whitebalance(float& r,float& g,float& b)
{
    int _nCurrent=get_current();
    r=ebus_camera[_nCurrent].lFilter.GetGainR();
    g=ebus_camera[_nCurrent].lFilter.GetGainG();
    b=ebus_camera[_nCurrent].lFilter.GetGainB();
    return 0;
}

int set_callback(eBusCameraCallBackFunction FUNC)
{
    ebus_camera[get_current()].cb_function=FUNC;
    return 0;
}

int unset_callback(void)
{
    ebus_camera[get_current()].cb_function=NULL;
    return 0;
}

int set_trigger_free(void)
{
    return auto_config("set_trigger_free");
}

int set_trigger_hardware(void)
{
    auto_config("set_trigger_hardware");
    return 0;
}

int get_trigger_mode(void)
{

    return -3;
}

int set_width(long int nWidth)
{
    return auto_config("set_width");
}
int get_width(long int& nHeight)
{
    if(0==get_device_int_value(nHeight,"Width")) return 0;
    else return -3;
}

int set_height(long int nHeight)
{
    return auto_config("set_height");
}

int get_height(long int& nHeight)
{
    if(0==get_device_int_value(nHeight,"Height")) return 0;
    return -3;
}

int set_exposure_abs(long int rexp,long int gexp,long int bexp)
{
    return auto_config("set_exposure_abs");
}

int get_exposure_abs(long int& rexp,long int& gexp,long int& bexp)
{
    return -3;
}

void* StreamProcessDispatch0(void* context)
{
    ebus_camera_type* camera=static_cast<ebus_camera_type*>(context);
    while(camera->bRunThread)
    {
        PvBuffer* pvbuf=0;
        PvResult retrieveRet;
        PvResult ret=camera->lStream.RetrieveBuffer(&pvbuf,&retrieveRet,500);
        if(ret.IsOK())
        {
            if(PvPayloadTypeImage==pvbuf->GetPayloadType())
            {
                std::fprintf(stderr, "%s\n","get image");
                int nLastRet(0);
                if(camera->cb_function)
                {
                    PvImage* img=pvbuf->GetImage();
                    PvBuffer* lBufferRGB32=new(std::nothrow) PvBuffer();
                    if(lBufferRGB32)
                    {
                        int _Width=img->GetWidth();
                        int _Height=img->GetHeight();
                        lBufferRGB32->GetImage()->Alloc(_Width,_Height,PvPixelWinRGB32);
                        PvBufferConverter lBufferConverter;
                        PvResult ftret=lBufferConverter.Convert(pvbuf,lBufferRGB32,true);
                        if(camera->valid_wb)
                        {
                            camera->lFilter.Apply(lBufferRGB32);
                        }
                        cv::Mat imgMat(lBufferRGB32->GetImage()->GetHeight(),lBufferRGB32->GetImage()->GetWidth(),CV_MAKETYPE(8,4),lBufferRGB32->GetDataPointer());
                        cv::Mat imgMat2;
                        cv::cvtColor(imgMat,imgMat2,CV_RGBA2RGB);
                        lBufferRGB32->GetImage()->Free();
                        delete lBufferRGB32;
                        lBufferRGB32=NULL;

                        EBUS_IMAGE image;
                        image.Width=imgMat2.cols;
                        image.Height=imgMat2.rows;
                        image.Depth=imgMat2.depth();
                        image.Channel=imgMat2.channels();
                        image.ImageSize=imgMat2.rows*imgMat2.step;
                        image.Timestamp=pvbuf->GetTimestamp();
                        image.pImageBuf=imgMat2.data;
                        nLastRet=camera->cb_function(&image);
                    }

                }
            }

            camera->lStream.QueueBuffer(pvbuf);
        }
    }
    return NULL;
}

void* StreamProcessDispatch1(void* context)
{
    ebus_camera_type* camera=static_cast<ebus_camera_type*>(context);
    while(camera->bRunThread)
    {
        PvBuffer* pvbuf=0;
        PvResult retrieveRet;
        PvResult ret=camera->lStream.RetrieveBuffer(&pvbuf,&retrieveRet,500);
        if(ret.IsOK())
        {
            if(PvPayloadTypeImage==pvbuf->GetPayloadType())
            {
                int nLastRet(0);
                if(camera->cb_function)
                {
                    PvImage* img=pvbuf->GetImage();
                    PvBuffer* lBufferRGB32=new(std::nothrow) PvBuffer();
                    if(lBufferRGB32)
                    {
                        int _Width=img->GetWidth();
                        int _Height=img->GetHeight();
                        lBufferRGB32->GetImage()->Alloc(_Width,_Height,PvPixelWinRGB32);
                        PvBufferConverter lBufferConverter;
                        PvResult ftret=lBufferConverter.Convert(pvbuf,lBufferRGB32,true);
                        if(camera->valid_wb)
                        {
                            camera->lFilter.Apply(lBufferRGB32);
                        }
                        cv::Mat imgMat(lBufferRGB32->GetImage()->GetHeight(),lBufferRGB32->GetImage()->GetWidth(),CV_MAKETYPE(8,4),lBufferRGB32->GetDataPointer());
                        cv::Mat imgMat2;
                        cv::cvtColor(imgMat,imgMat2,CV_RGBA2RGB);
                        lBufferRGB32->GetImage()->Free();
                        delete lBufferRGB32;
                        lBufferRGB32=NULL;

                        EBUS_IMAGE image;
                        image.Width=imgMat2.cols;
                        image.Height=imgMat2.rows;
                        image.Depth=imgMat2.depth();
                        image.Channel=imgMat2.channels();
                        image.ImageSize=imgMat2.rows*imgMat2.step;
                        image.Timestamp=pvbuf->GetTimestamp();
                        image.pImageBuf=imgMat2.data;
                        nLastRet=camera->cb_function(&image);
                    }

                }
            }

            camera->lStream.QueueBuffer(pvbuf);
        }
    }
    return NULL;
}

void* StreamProcessDispatch2(void* context)
{
    ebus_camera_type* camera=static_cast<ebus_camera_type*>(context);
    while(camera->bRunThread)
    {
        PvBuffer* pvbuf=0;
        PvResult retrieveRet;
        PvResult ret=camera->lStream.RetrieveBuffer(&pvbuf,&retrieveRet,500);
        if(ret.IsOK())
        {
            if(PvPayloadTypeImage==pvbuf->GetPayloadType())
            {
                int nLastRet(0);
                if(camera->cb_function)
                {
                    PvImage* img=pvbuf->GetImage();
                    PvBuffer* lBufferRGB32=new(std::nothrow) PvBuffer();
                    if(lBufferRGB32)
                    {
                        int _Width=img->GetWidth();
                        int _Height=img->GetHeight();
                        lBufferRGB32->GetImage()->Alloc(_Width,_Height,PvPixelWinRGB32);
                        PvBufferConverter lBufferConverter;
                        PvResult ftret=lBufferConverter.Convert(pvbuf,lBufferRGB32,true);
                        if(camera->valid_wb)
                        {
                            camera->lFilter.Apply(lBufferRGB32);
                        }
                        cv::Mat imgMat(lBufferRGB32->GetImage()->GetHeight(),lBufferRGB32->GetImage()->GetWidth(),CV_MAKETYPE(8,4),lBufferRGB32->GetDataPointer());
                        cv::Mat imgMat2;
                        cv::cvtColor(imgMat,imgMat2,CV_RGBA2RGB);
                        lBufferRGB32->GetImage()->Free();
                        delete lBufferRGB32;
                        lBufferRGB32=NULL;

                        EBUS_IMAGE image;
                        image.Width=imgMat2.cols;
                        image.Height=imgMat2.rows;
                        image.Depth=imgMat2.depth();
                        image.Channel=imgMat2.channels();
                        image.ImageSize=imgMat2.rows*imgMat2.step;
                        image.Timestamp=pvbuf->GetTimestamp();
                        image.pImageBuf=imgMat2.data;
                        nLastRet=camera->cb_function(&image);
                    }

                }
            }

            camera->lStream.QueueBuffer(pvbuf);
        }
    }
    return NULL;
}

void* StreamProcessDispatch3(void* context)
{
    ebus_camera_type* camera=static_cast<ebus_camera_type*>(context);
    while(camera->bRunThread)
    {
        PvBuffer* pvbuf=0;
        PvResult retrieveRet;
        PvResult ret=camera->lStream.RetrieveBuffer(&pvbuf,&retrieveRet,500);
        if(ret.IsOK())
        {
            if(PvPayloadTypeImage==pvbuf->GetPayloadType())
            {
                int nLastRet(0);
                if(camera->cb_function)
                {
                    PvImage* img=pvbuf->GetImage();
                    PvBuffer* lBufferRGB32=new(std::nothrow) PvBuffer();
                    if(lBufferRGB32)
                    {
                        int _Width=img->GetWidth();
                        int _Height=img->GetHeight();
                        lBufferRGB32->GetImage()->Alloc(_Width,_Height,PvPixelWinRGB32);
                        PvBufferConverter lBufferConverter;
                        PvResult ftret=lBufferConverter.Convert(pvbuf,lBufferRGB32,true);
                        if(camera->valid_wb)
                        {
                            camera->lFilter.Apply(lBufferRGB32);
                        }
                        cv::Mat imgMat(lBufferRGB32->GetImage()->GetHeight(),lBufferRGB32->GetImage()->GetWidth(),CV_MAKETYPE(8,4),lBufferRGB32->GetDataPointer());
                        cv::Mat imgMat2;
                        cv::cvtColor(imgMat,imgMat2,CV_RGBA2RGB);
                        lBufferRGB32->GetImage()->Free();
                        delete lBufferRGB32;
                        lBufferRGB32=NULL;

                        EBUS_IMAGE image;
                        image.Width=imgMat2.cols;
                        image.Height=imgMat2.rows;
                        image.Depth=imgMat2.depth();
                        image.Channel=imgMat2.channels();
                        image.ImageSize=imgMat2.rows*imgMat2.step;
                        image.Timestamp=pvbuf->GetTimestamp();
                        image.pImageBuf=imgMat2.data;
                        nLastRet=camera->cb_function(&image);
                    }

                }
            }

            camera->lStream.QueueBuffer(pvbuf);
        }
    }
    return NULL;
}

typedef void* (*StreamProcessDispatch_Type)(void*);

static StreamProcessDispatch_Type StreamProcessDispatch[]=
{
    StreamProcessDispatch0,StreamProcessDispatch1,StreamProcessDispatch2,StreamProcessDispatch3
};

int grab()
{
    int _nCurrent=get_current();
    if(!ebus_camera[_nCurrent].lDevice.IsConnected())
    {
        std::fprintf(stderr,"[ebuscamera.so] Device Is Not Connected.\n");
        return -3;
    }
    PvGenParameterArray* lDeviceParams=ebus_camera[_nCurrent].lDevice.GetGenParameters();
    ebus_camera[_nCurrent].lTLLocked=dynamic_cast<PvGenInteger*>(lDeviceParams->Get("TLParamsLocked"));
    PvGenCommand* lStart=dynamic_cast<PvGenCommand*>(lDeviceParams->Get("AcquisitionStart"));
    if(0==ebus_camera[_nCurrent].lTLLocked)
    {
        ebus_camera[_nCurrent].lTLLocked->SetValue(1);
    }
    PvGenCommand *lResetTimestamp=dynamic_cast<PvGenCommand*>(lDeviceParams->Get("GevTimestampControlReset"));
    lResetTimestamp->Execute();

    //start thread

    pthread_attr_t ThreadAttr;
    pthread_attr_init(&ThreadAttr);
    pthread_attr_setschedpolicy(&ThreadAttr,SCHED_RR);
    if(ebus_camera[_nCurrent].bRunThread)
    {
        ebus_camera[_nCurrent].bRunThread=false;
    }
    ebus_camera[_nCurrent].bRunThread=true;
    pthread_create(&ebus_camera[_nCurrent].hStreamThread,&ThreadAttr,StreamProcessDispatch[_nCurrent],&ebus_camera[_nCurrent]);

    if(lStart->Execute().IsOK()) return 0;
    else return -3;
}

int stop()
{
    ebus_camera[get_current()].bRunThread=false;
    if(ebus_camera[get_current()].hStreamThread)
    {
        pthread_join(ebus_camera[get_current()].hStreamThread,NULL);
        ebus_camera[get_current()].hStreamThread=0;
    }
    if(!ebus_camera[get_current()].lDeviceRet.IsOK())
    {
        std::fprintf(stderr, "%s\n","[ebuscamera.so] Device Is Not Connected.");
        return -3;
    }
    PvResult retval(dynamic_cast<PvGenCommand*>(ebus_camera[get_current()].lDevice.GetGenParameters()->Get("AcquisitionStop"))->Execute());
    if(retval.IsOK()) return 0;else return -3;
    if(0!=ebus_camera[get_current()].lTLLocked)
    {
        ebus_camera[get_current()].lTLLocked->SetValue(0);
    }
    return 0;
}

