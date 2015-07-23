#ifndef __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_CALLBACK_H__
#define __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_CALLBACK_H__

#include "ebuscam.h"
#include <vector>


enum image_type
{
    normal          =0,
    calibrate_board =1,
    correct         =2,
};

void reset_board_index();

void set_shm(std::vector<long>& input_shm_key,int nCount);

void set_rect(int whichRect,int32_t x,int32_t y,int32_t x2,int32_t y2);

int callback_func(EBUS_IMAGE* img);

void set_save_image(image_type type,int param=0);

#endif /* __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_CALLBACK_H__ */