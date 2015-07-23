#include "modbusnet.h"

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


ModBusNet::ModBusNet(boost::asio::io_service& ios
                    ,const std::string& ip
                    ,const int port)
                    :_ios(ios)
                    ,_ep(boost::asio::ip::address::from_string(ip),port)
                    ,_socket(new boost::asio::ip::tcp::socket(_ios))
{
    ModBusNet::NewConnect();
}

ModBusNet::~ModBusNet()
{
    try
    {
        this->_socket->close();
    }
    catch(std::exception& e)
    {
        std::fprintf(stderr, "[**** ERROR **]:%s\n",e.what());
    }
}

void ModBusNet::NewConnect()
{
    this->_socket->async_connect(this->_ep
                                ,boost::bind(&ModBusNet::OnConnnect
                                            ,this,boost::asio::placeholders::error
                                            ,0
                                            ,false)
                                );
}

void ModBusNet::OnConnnect(const boost::system::error_code& ec
                                            ,const unsigned long long int ImageIdentity
                                            ,const bool bReConnect)
{
    static bool show_error(true);
    if(ec)
    {
        if(show_error)
        {
            std::fprintf(stderr, "%s\n","[**** ERROR **]:Failed To Connect ModBus Server");
            show_error=false;
        }

        return;
    }
    show_error=true;
    if(!bReConnect)
    {
        std::fprintf(stderr, "%s\n","[**** NOTICE  ]:SetPic New ModBusNet Client Connection Created");
    }
    else
    {
        std::fprintf(stderr, "%s\n","[**** NOTICE  ]:SetPic ModBusNet Has ReConnected ,Send Image Identity .");
    }
    if(this->_socket->is_open() && true==bReConnect && 0!=ImageIdentity)
    {
        std::fprintf(stderr, "%s\n","OnConnnect resend !");
        ModBusNet::AsyncWriteSome(ImageIdentity);
    }
}

void ModBusNet::AsyncWriteSome(const unsigned long long int ImageIdentity)
{
    if(this->_ios.stopped())
    {
        std::fprintf(stderr, "%s\n","[**** ERROR   ]: IO SERVEICE IS STOPED ! Will Exit ...");
        return ;
    }
    char wbuf[64];std::memset(wbuf,0x00,sizeof(wbuf));
    std::sprintf(wbuf,"getpic:%lld\r\n",ImageIdentity);
    if(this->_socket->is_open())
    {
        this->_socket->async_write_some(boost::asio::buffer(wbuf,sizeof(wbuf))
                                        ,boost::bind(&ModBusNet::OnWriteDone
                                                    ,this
                                                    ,boost::asio::placeholders::error
                                                    ,ImageIdentity)
                                        );
    }
    else
    {
        this->_socket->async_connect(this->_ep
                                    ,boost::bind(&ModBusNet::OnConnnect
                                                ,this
                                                ,boost::asio::placeholders::error
                                                ,ImageIdentity
                                                ,true)
                                    );
    }
}

void ModBusNet::OnWriteDone(const boost::system::error_code& ec,const unsigned long long int ImageIdentity)
{
    if(ec)
    {
        std::fprintf(stderr, "%s TimeStamp=%lld\n","[**** ERROR **]:Send ImageIdentity ERROR !",ImageIdentity);
        std::fprintf(stderr, "%s%s\n","[**** ERROR **]:",boost::system::system_error(ec).what());
        try
        {
            this->_socket->close();
            ModBusNet::AsyncWriteSome(ImageIdentity);
        }
        catch(std::exception& e)
        {
            std::fprintf(stderr, "[**** ERROR **]:%s\n",e.what());
        }
    }
}
