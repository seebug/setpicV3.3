#ifndef __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_MODBUSNET_H__
#define __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_MODBUSNET_H__

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "global.h"


class ModBusNet
{
public:
    ModBusNet(boost::asio::io_service& ios, const std::string& ip,const int port);
    virtual ~ModBusNet(void);

public:
    void NewConnect(void);
    void OnConnnect(const boost::system::error_code& ec,const unsigned long long int ImageIdentity,const bool bReConnect);
    void AsyncWriteSome(const unsigned long long int ImageIdentity);
    void OnWriteDone(const boost::system::error_code& ec,const unsigned long long int ImageIdentity);
    void AsyncReadSome();

private:
    boost::asio::io_service&                                        _ios;
    boost::asio::ip::tcp::endpoint                                  _ep;
    boost::shared_ptr<boost::asio::ip::tcp::socket>                 _socket;
};

#endif /* __WANGBIN_SETPIC33_MODULES_SETPIC_INCLUDE_MODBUSNET_H__ */