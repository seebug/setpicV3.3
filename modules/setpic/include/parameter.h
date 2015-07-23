#ifndef __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_PARAMETER_H__
#define __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_PARAMETER_H__

#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include "global.h"

bool init_parameter(int argc,char** argv);

bool uninit_parameter(void);

bool check_help(void);

bool check_is_default(const char* option);

void print_help(void);

bool get_parameter_int(const char* option,int& retval);

bool get_parameter_string(const char* option,std::string& retval);

bool get_parameter_unrecongnized(std::vector<std::string>& retval);



#endif /* __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_PARAMETER_H__ */