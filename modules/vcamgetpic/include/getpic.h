#ifndef __WANGBIN_SETPIC33_MODULES_VCAMGETPIC_INCLUDE_GETPIC_H__
#define __WANGBIN_SETPIC33_MODULES_VCAMGETPIC_INCLUDE_GETPIC_H__

#include <cstdlib>
#include <cstring>

#include <opencv2/opencv.hpp>

bool init_getpic(const char* rt_path);

bool init_shared_memory(const int count=2);

bool get_image(cv::Mat& output_src,cv::Mat& output,int& total,int& classid,int& grab_time,unsigned long long int& ulldTimeStamp,cv::Rect* outLeft,cv::Rect* outMiddle,cv::Rect* outRight,int& nDataCorrct,int& nDataInit,int& nTriggerMode);

bool uninit_shared_memory(void);

// test
/* 从共享内存中读取原始图像，
* 该函数不会改变用于标记客户端是否已经读取的状态，而是使用扩展的子段标记状态，
* 仅用于test_setpic测试
*/
bool get_image_test(cv::Mat& output_src,unsigned long long int& ulldTimeStamp);

#endif /* __WANGBIN_SETPIC33_MODULES_VCAMGETPIC_INCLUDE_GETPIC_H__ */