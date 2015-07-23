#ifndef __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_SETPIC_H__
#define __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_SETPIC_H__

#include "global.h"

#include <cstring>
#include <string>

extern bool global_is_exit;

std::string GetVersionString();

void on_signal_sigint(int sign_no);

void on_signal_sigquit(int sign_no);

void on_signal_sigterm(int sign_no);

void on_signal_sigsegv(int sign_no);

bool check_info();

#endif /* __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_SETPIC_H__ */