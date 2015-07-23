#ifndef PARTITIONCOLORCORRECT_H
#define PARTITIONCOLORCORRECT_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

//cv::Mat color_correct(cv::Mat&,int );
cv::Mat PartitionColorCorrect(cv::Mat&);
//参数2：全白板图片。
//参数3：盖镜头盖图片
//参数4：选择校正模式，0为只进行色彩校正，1为只进行空间校正，2为同时进行色彩校正和空间校正。
//参数5：计算矩阵的产生方式，0表示读入XML内数据，1表示重新计算。
//参数6:输出的暗电流的值
//参数7：输出的白平衡系数的值

#endif
