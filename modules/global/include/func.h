#ifndef __WANGBIN_SETPIC33_MODULES_GLOBAL_INCLUDE_FUNC_H__
#define __WANGBIN_SETPIC33_MODULES_GLOBAL_INCLUDE_FUNC_H__

#ifdef HAVE_CXX11_SUPPORT
#   define AutoMove(X)     std::move(X)
#else
#   define AutoMove(x)      (x)
#   define nullptr          0
#endif

#endif /* __WANGBIN_SETPIC33_MODULES_GLOBAL_INCLUDE_FUNC_H__ */