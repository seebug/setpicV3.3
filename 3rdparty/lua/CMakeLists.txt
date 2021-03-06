set(the_description "The Lua Library")
# 添加源代码文件
aux_source_directory(src NEBULA_LUA_SOURCE_CODE)
# 设置目标名称
set(NEBULA_LUA_LIBRARY lua)

# 检查目标使用的头文件
include(CheckIncludeFile)
check_include_file(readline/readline.h LUA_HAVE_READLINE_READLINE_H)

# 添加相关宏定义
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

add_definitions(-DLUA_COMPAT_5_2)

if(UNIX)
    add_definitions(-DLUA_USE_LINUX)
    if(CMAKE_COMPILER_IS_GUNCXX)
        set(CMAKE_C_FLAG "${CMAKE_C_FLAGS} -fPIC")
    endif(CMAKE_COMPILER_IS_GUNCXX)
elseif(WIN32)
    add_definitions(-D_WIN32)
endif(UNIX)

#nebula_include_directories("${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

# 确定编译目标类型
set(BUILD_TYPE_LUA "SHARED")

if(BUILD_STATIC_LUA)
    set(BUILD_TYPE_LUA  "STATIC")
else()
    if(WIN32 OR WIN64)
        add_definitions(-DLUA_BUILD_AS_DLL)
    endif(WIN32 OR WIN64)
endif(BUILD_STATIC_LUA)

# 编译目标
add_library(${NEBULA_LUA_LIBRARY} ${BUILD_TYPE_LUA} ${NEBULA_LUA_SOURCE_CODE})

# 编译器标准
if(HAVE_COMPILER_STD_FLAG_GUN99_SUPPORT)
    set(CMAKE_C_FLAG "${CMAKE_C_FLAGS} ${NEBULA_COMPILER_STD_FLAG_GUN99}")
endif(HAVE_COMPILER_STD_FLAG_GUN99_SUPPORT)

# 目标属性
set_target_properties(${NEBULA_LUA_LIBRARY} PROPERTIES DEFINE_SYMBOL NEBULA_LUA_EXPORT)

set_target_properties(${NEBULA_LUA_LIBRARY} PROPERTIES
        OUTPUT_NAME ${NEBULA_LUA_LIBRARY}
        ARCHIVE_OUTPUT_DIRECTORY ${3P_LIBRARY_OUTPUT_PATH}
    )

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${NEBULA_LUA_LIBRARY} PROPERTIES FOLDER "3rdparty")
endif()


