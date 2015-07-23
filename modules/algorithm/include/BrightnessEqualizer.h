#ifndef BRIGHTNESSEQUALIZER_H
#define BRIGHTNESSEQUALIZER_H

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
#include "cv.h"
//cv::Mat color_correct(cv::Mat&,int );
cv::Mat BrightnessEqualizer(cv::Mat& );
#endif
