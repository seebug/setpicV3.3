#ifndef __WANGBIN_SETPIC33_MODULES_VCAMERA_INCLUDE_VCAMERA_H__
#define __WANGBIN_SETPIC33_MODULES_VCAMERA_INCLUDE_VCAMERA_H__

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include "ebuscam.h"

struct image_mode_st
{
    std::string     train;
    std::string     run;
    std::string     xpressive;
    int             count;
    int             section;
    int             left;
    int             times;
};


int _load_config(void* lpszConfig);

int _unload_config(void);

int _restore_config(void);

int _open();

int _close();

int _set_buffer();

int _unset_buffer();

int _set_current(int);

int _get_current(void);

int _set_whitebalance(float r,float g,float b);

int _get_whitebalance(float& r,float& g,float& b);

int _set_callback(eBusCameraCallBackFunction FUNC);

int _unset_callback(void);

int _set_trigger_free(void);

int _set_trigger_hardware(void);
//int get_trigger_mode(void);

int _set_width(long int nWidth);

int _get_width(long int& nHeight);

int _set_height(long int nHeight);

int _get_height(long int& nHeight);

int _set_exposure_abs(long int rexp,long int gexp,long int bexp);

int _get_exposure_abs(long int& rexp,long int& gexp,long int& bexp);

int _grab();

int _stop();

#endif /* __WANGBIN_SETPIC33_MODULES_VCAMERA_INCLUDE_VCAMERA_H__ */