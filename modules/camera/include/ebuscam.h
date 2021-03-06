#ifndef __WANGBIN_SETPIC33_MODULES_CAMERA_INCLUDE_H__
#define __WANGBIN_SETPIC33_MODULES_CAMERA_INCLUDE_H__

#include <cstdlib>
#include <cstdio>

#include <stdint.h>


/*
===================================================================================================
function list:

int load_config(const char* lpszConfig);
int unload_config(void);
int restore_config(void);
int open();
int close();
int set_buffer();
int unset_buffer();
int set_current(int);
int get_current(void);
int set_whitebalance(float r,float g,float b);
int get_whitebalance(float& r,float& g,float& b);
int set_callback(eBusCameraCallBackFunction FUNC);
int unset_callback(void);
int set_trigger_free(void);
int set_trigget_hardware(void);
//int get_trigger_mode(void);
int set_width(long int nWidth);
int get_width(long int& nHeight);
int set_height(long int nHeight);
int get_height(long int& nHeight);
int set_exposure_abs(long int rexp,long int gexp,long int bexp);
int get_exposure_abs(long int& rexp,long int& gexp,long int& bexp);
int grab();
int stop();
===================================================================================================
*/

typedef struct _EBUS_IMAGE_TYPE
{
    int32_t     Width;                  //宽度
    int32_t     Height;                 //高度
    int32_t     Depth;                  //深度
    int32_t     Channel;                //通道数
    void*       pImageBuf;              //图像地址
    int32_t     ImageSize;              //图像像素数量
    uint64_t    Timestamp;              //时间戳
} EBUS_IMAGE;

typedef enum _EBUS_EM_TRIGGER_MODE
{
    TRIGGER_FREE=0,                     //自由触发
    TRIGGER_HARDWARE=1,                 //硬件触发
} TRIGGER_MODE;

typedef int (*eBusCameraCallBackFunction)(EBUS_IMAGE* eImage);

//*************************************************************************************************
/// \brief                          :加载配置文件
///
/// \param [in] lpszConfig          :配置文件路径（相对路径或绝对路径）
///
/// \retval                         :0 表示成功，其它数字 表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :restore_config,unload_config
///
int load_config(const char* lpszConfig);


//*************************************************************************************************
/// \brief                          :卸载配置文件
///
/// \retval                         :0 表示成功，其它数字 表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :load_config,restore_config
int unload_config(void);

//*************************************************************************************************
/// \brief                          :恢复已经加载的配置文件
///
/// \retval                         :0 表示成功，其它数字 表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///                                 【注意】恢复配置文件实际上是恢复设备链接及其数据流，使用前请确保配置文件已经
///                                 加载，另外，需要特别注意，配置文件在恢复链接时使用的是配置文件中所指定的设备
///                                 物理地址，因而，配置文件中的物理地址需要与将要使用的设备物理地址一致。
///
/// \sa                             :load_config,unload_config
int restore_config(void);

//*************************************************************************************************
/// \brief                          :打开数据设备
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///                                 该函数内部执行的实际操作是：打开数据流，因而在执行操作前需要确保配置文件已经
///                                 成功恢复，如果没有，可以使用 restore_config 进行恢复。有关 restore_config
///                                 使用说明，请参考 restore_config
///
/// \sa                             :restore_config,close
int open();

//*************************************************************************************************
/// \brief                          :关闭数据设备
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///                                 函数内部执行的实际操作是关闭数据流，因而改函数要与打开数据流 open 成对使用
///
/// \sa                             :open
int close();

//*************************************************************************************************
/// \brief                          :设置数据缓存
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///                                 设置缓存时，函数内部会自动分配一定空间，并放入相机缓存队列。另外，函数也会执
///                                 行其它的一些操作，如设置像素格式，数据位，第一张图像的超时时间等。
///
/// \sa                             :unset_buffer
int set_buffer();

//*************************************************************************************************
/// \brief                          :删除数据缓存
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :set_buffer
int unset_buffer();

//*************************************************************************************************
/// \brief                          :设置当前将要进行设置的相机索引
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :
///
/// \sa                             :get_current
int set_current(int);

//*************************************************************************************************
/// \brief                          :获取当前正在设置的相机索引值
///
/// \retval                         :当前正在设置的相机索引值
///
/// \note                           :
///
/// \sa                             :set_current
int get_current(void);

//*************************************************************************************************
/// \brief                          :设置白平衡参数
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :get_whitebalance
int set_whitebalance(float r,float g,float b);

//*************************************************************************************************
/// \brief                          :获取白平衡参数
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :set_whitebalance
int get_whitebalance(float& r,float& g,float& b);

//*************************************************************************************************
/// \brief                          :注册回调函数
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :unset_callback
int set_callback(eBusCameraCallBackFunction FUNC);

//*************************************************************************************************
/// \brief                          :注销回调函数
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :set_callback
int unset_callback(void);

//*************************************************************************************************
/// \brief                          :设置自由触发
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///                                 【注意】自由模式是否工作并不完全依赖于软件设置，部分采集盒需要在帧有效的情况
///                                 下才可以。
///
/// \sa                             :set_trigget_hardware
int set_trigger_free(void);

//*************************************************************************************************
/// \brief                          :设置硬件触发
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :set_trigger_free
int set_trigger_hardware(void);
//int get_trigger_mode(void);

//*************************************************************************************************
/// \brief                          :设置图像宽度
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///                                 【注意】根据相机的实际分辨率设置图像宽度，否则图像会被自动截取
///
/// \sa                             :get_width
int set_width(long int nWidth);

//*************************************************************************************************
/// \brief                          :获取图像宽度
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :set_width
int get_width(long int& nHeight);

//*************************************************************************************************
/// \brief                          :设置图像高度
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :get_height
int set_height(long int nHeight);

//*************************************************************************************************
/// \brief                          :获取图像配置文件中设置的高度
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :set_height
int get_height(long int& nHeight);

//*************************************************************************************************
/// \brief                          :获取相机曝光时间
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :get_exposure_abs
int set_exposure_abs(long int rexp,long int gexp,long int bexp);

//*************************************************************************************************
/// \brief                          :设置相机曝光时间
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :set_exposure_abs
int get_exposure_abs(long int& rexp,long int& gexp,long int& bexp);

//*************************************************************************************************
/// \brief                          :开始抓取图像
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :close
int grab();

//*************************************************************************************************
/// \brief                          :停止抓取图像
///
/// \retval                         :0 表示成功，否则表示失败
///
/// \note                           :函数会根据设置的相机索引进行加载，设置前可以使用set_current函数进行相机选
///                                  择或者使用get_current获得当前正在关联的相机索引值
///
/// \sa                             :grab
int stop();

#endif /* __WANGBIN_SETPIC33_MODULES_CAMERA_INCLUDE_H__ */