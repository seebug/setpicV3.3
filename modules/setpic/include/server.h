#ifndef __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_SERVER_H__
#define __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_SERVER_H__

#include "global.h"

#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>

#include <stdint.h>

#include <boost/asio.hpp>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/typeof/typeof.hpp>


#include "ebuscam.h"
//#include "modbusnet.h"

#define CAMERA_MODE     1
#define IMAGE_MODE      0

struct tcp_st
{
    int32_t     id;
    int32_t     mode;
    int32_t     ret;
    char    msg[256];
};

#define CMD_TRIGGER_MODE            1       //int
#define CMD_EXPOSURE_TIME           2       //long long int
#define CMD_GAIN                    3       //long long int
#define CMD_WHITE_BALANCE           4       //long long int,order:r,g,b
#define CMD_COLOR_CORRECT           5       //float ,r,g,b
#define CMD_CALI_CORRECT           6       //0 HEI 1 BAI
#define CMD_DISTORTION              7       //
#define CMD_FOCUS                   8       //double
#define CMD_FOR_INIT_VALUE          9       //width,height,triggermode,min exposuretime,exposure time, max exposuretime,min gain,gain,max gain,r gain,g gain,b gain
#define CMD_CALIBRATE_IMAGE         10      //calibrate image
#define CMD_LEFT_RECT              11      //INT
#define CMD_MIDDLE_RECT             12      //INT
#define CMD_RIGHT_RECT              13      //INT
#define CMD_FOCUS_VALUE             14      //DOUBLE
#define CMD_NET_TEST                15      //
#define CMD_SAVE_IMAGE              16      //0 NOT SAVE 1 SAVE
#define CMD_SEQ_START               17      //NEW SEQ START
#define CMD_GET_IMGAE_WIDTH_HEIGHT  18      //Get Image Width and Height
#define CMD_RESET_CORECT_RECT       19      //Rest Rect to 0,0,0,0
#define CMD_GET_VERSION_STRING      20
#define CMD_CLOSE_SETPIC            21      //
#define CMD_MAKE_CALIBRATE_XML      22      //GET XML
#define CMD_SEQ_STOP                23
#define CMD_SOFTTRIGGER_ONCE        24      //Software Trigger Once
#define CMD_IMAGE_PROCESS_SET       25      //0 FOR Original Image Push Shared Memory
#define CMD_COUNTER_SWITCH         26      //FOR Trigger Vount ,1 for begin ,0 for end and not counter
#define CMD_COUNTER_DATA         27      //For Camera Connect Counter, 1 for begin 0 for end and not counter
#define CMD_WHITE_BALANCE_EXPOSURE_RGB_GAIN_MRB 28  //line scan whitebalance and set gain
#define CMD_BALIBRATE_IMAGE_AND_XML                29  //line scan calibrate xml update


class setpic_server
{
public:
    explicit setpic_server(const void* config);
    virtual ~setpic_server(void) throw();

public:
    void on_acceptor(const boost::system::error_code& err
                    ,boost::shared_ptr<boost::asio::ip::tcp::socket> server_socket);

    bool new_connnect(void);


    void on_read_done(const boost::system::error_code& err
                            ,size_t get_count
                            ,boost::shared_ptr<tcp_st> read_buffer
                            ,boost::shared_ptr<tcp_st> write_buffer
                            ,boost::shared_ptr<boost::asio::ip::tcp::socket> server_socket);

    void on_write_done(const boost::system::error_code& error
                            ,size_t write_count
                            ,boost::shared_ptr<tcp_st> read_buffer
                            ,boost::shared_ptr<tcp_st> write_buffer
                            ,boost::shared_ptr<boost::asio::ip::tcp::socket> server_socket);

    void operator_camera(boost::shared_ptr<tcp_st> input,
                                    boost::shared_ptr<tcp_st> output);

    static void run(void);
    static void stop(void);
private:
public:
    static int                              tcp_mode;
    static boost::asio::io_service          m_ios;
    boost::asio::io_service::strand         m_strand;
    boost::asio::ip::tcp::acceptor          m_acceptor;

};

void setpic_tcp_socket(void);

#endif /* __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_SERVER_H__ */