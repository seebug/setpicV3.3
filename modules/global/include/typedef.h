#ifndef __WANGBIN_SETPIC33_MODULES_GLOBAL_INCLUDE_TYPEDEF_H__
#define __WANGBIN_SETPIC33_MODULES_GLOBAL_INCLUDE_TYPEDEF_H__

#include <cstdlib>
#include <cstdio>
#include <opencv2/opencv.hpp>

enum _V_C_ENUM_PUSH_MEMORY_STATUS_
{
    RUNNING             =0,         //DEFAULT ,USE BUT NOT BUSY,CAN READ AND WRITE
    FREE                =1,         //NEVER USED BEFORE
    LOCK_WRITING        =2,         //ON WRITING
    SAFT_READING        =3,
    LOCK_READING        =4,         //ON READING
    SAFT_DELETED        =5,
};

#define DEF_FILE_PATH_LEN 256
//#define DEF_IMAGE_DATA_SIZE 2058*2456*3
#define DEF_IMAGE_DATA_SIZE 4096*5000*3+4096

struct shm_st
{
    unsigned int                                    nTotal;                                         //total image when using image,but 0 for camera
    unsigned long long int                          nState;                                     //Lock State
    unsigned int                                    nRWState;                                    //Writing state
    unsigned int                                    nCmdState;                                     //Reading state
    unsigned int                                    uPixelType;                                     //Pixel Type
    unsigned int                                    uSizeX;                                         //width
    unsigned int                                    uSizeY;                                         //height
    unsigned int                                    uImageSize;                                     //Image Size
    unsigned int                                    uMissingPackets;                                //Missing Packets
    unsigned int                                    uAnnouncedBuffers;                              //Announced Buffers
    unsigned int                                    uQueuedBuffers;                                 //Queued Buffers
    unsigned int                                    uOffsetX;                                       //Offset X
    unsigned int                                    uOffsetY;                                       //Offset Y
    unsigned int                                    uAwaitDelivery;                                 //Await Delivery
    unsigned int                                    uBlockID;                                       //Block id
    unsigned long long int                          ulldTimeStamp;                                  //TimeStamp
    int                                             nClassID;                                       //Classifier
    int                                             nGrabTime;                                      //Grab Image Time
    char                                            szFileName[DEF_FILE_PATH_LEN];                  //File Path,In reading Or writing
    char                                            szImageData[DEF_IMAGE_DATA_SIZE];               //szImageData
    char                                            szImageDataSrc[DEF_IMAGE_DATA_SIZE];
    cv::Rect                                        LeftRect;
    cv::Rect                                        MiddleRect;
    cv::Rect                                        RightRect;
    int                                             DataCorrct; //0 success , -1 failed
    int                                             DataInit;   //0 success ,-1 failed
    int                                             TriggerMode;
    int                                             ExtendIntA;
};


#endif /* __WANGBIN_SETPIC33_MODULES_GLOBAL_INCLUDE_TYPEDEF_H__ */