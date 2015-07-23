#ifndef __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_RTMEMORY_H__
#define __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_RTMEMORY_H__

#include <cstdlib>
#include <cstring>
#include <vector>

#include "global.h"

extern std::vector<long> vShmKey;


void init_dir_runtime(const int nSection,const char* sk);

void init_config(const void* pconfig=nullptr);


#endif /* __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_RTMEMORY_H__ */