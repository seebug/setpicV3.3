#!/usr/bin/env bash

#g++ -o libvcamgetpic.so -fpic -fPIC -shared getpic.cpp -D_SETPIC_IPC_FLAG_=0x205 -lrt -ldl -I./../include -I./../../script/include -I./../../global -I./../../../3rdparty/lua/src ./../../../build/libs/libscript.a ./../../../build/3rdparty/liblua.a /usr/local/lib/libboost_system.a /usr/local/lib/libboost_filesystem.a

g++ -o libvcamgetpic.so -fpic -fPIC -shared getpic.cpp -D_SETPIC_IPC_FLAG_=0x205 -lrt -ldl -I./../include -I./../../script/include -I./../../global -I./../../../3rdparty/lua/src ./../../../build/libs/libscript.a ./../../../build/3rdparty/liblua.a /usr/lib/i386-linux-gnu/libboost_system.a /usr/lib/i386-linux-gnu/libboost_filesystem.a `pkg-config opencv --libs --cflags opencv`

g++ -o test_getpic ./libvcamgetpic.so -I./../include test_getpic.cpp `pkg-config opencv --libs --cflags opencv` -lrt -ldl