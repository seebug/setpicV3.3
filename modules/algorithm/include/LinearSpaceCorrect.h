#ifndef LINEARSPACECORRECT_H
#define LINEARSPACECORRECT_H

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

void SpaceCorrect(cv::Mat& ImC,cv::Mat& ImCblne,cv::Mat& ImW,cv::Mat& dark, cv::Mat& Im_weight, cv::Scalar& dl,cv::Scalar& gain,int ifread);
//void spacial_calib(cv::Mat& imgC, cv::Mat& Im_weight, cv::Scalar bb, cv::Mat& ImCblne);
//cv::Mat correct_line(cv::Mat&,cv::Mat&,cv::Mat&,  cv::Scalar&,cv::Scalar&,int);//参数1：读入原始瓷砖图片，
//void spacial_init(cv::Mat& ImW, cv::Mat& dark, cv::Mat& Im_weight, cv::Scalar& dl, cv::Scalar& gain, int ifread);
//参数1：读入原始瓷砖图片，
//参数2：全白板图片。
//参数3：盖镜头盖图片
//参数4：计算矩阵的产生方式，0表示读入XML内数据，1表示重新计算。
//参数5: 输出的暗电流的值
//参数6：输出的白平衡系数的值
void check(cv::Mat& ,int ,int ,uchar &);

#endif
