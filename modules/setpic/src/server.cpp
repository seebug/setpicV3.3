#include "server.h"
#include "callback.h"

int                         setpic_server::tcp_mode(CAMERA_MODE);
boost::asio::io_service     setpic_server::m_ios(2);

static bool global_is_tcp_run(false);
extern bool global_is_exit;

setpic_server::setpic_server(const void* config)
            :m_strand(m_ios)
            ,m_acceptor(m_ios,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),9600))
{
    if(m_ios.stopped())
    {
        std::fprintf(stderr, "%s\n","io service is stopped !");
    }
    else
    {
        new_connnect();
    }

}

setpic_server::~setpic_server() throw()
{

}

void setpic_server::on_acceptor(const boost::system::error_code& err,boost::shared_ptr<boost::asio::ip::tcp::socket> server_socket)
{
    new_connnect();
    if(m_ios.stopped())
    {
        return;
    }
    if(err)
    {
        std::fprintf(stderr, "%s\n",boost::system::system_error(err).what());
        return;
    }
    std::fprintf(stderr, "client ip %s\n",server_socket->remote_endpoint().address().to_string().c_str());

    boost::shared_ptr<tcp_st> read_buffer(boost::make_shared<tcp_st>());
    boost::shared_ptr<tcp_st> write_buffer(boost::make_shared<tcp_st>());

    server_socket->async_read_some(boost::asio::buffer(read_buffer.get(),sizeof(tcp_st))
                                    ,boost::bind(&setpic_server::on_read_done
                                    ,this
                                    ,boost::asio::placeholders::error
                                    ,boost::asio::placeholders::bytes_transferred
                                    ,read_buffer
                                    ,write_buffer
                                    ,server_socket)
                                    );


}

bool setpic_server::new_connnect()
{
    if(m_ios.stopped())
    {
        std::fprintf(stderr, "%s\n","io service is stopped !");
        return false;
    }
    boost::shared_ptr<boost::asio::ip::tcp::socket> server_socket(new boost::asio::ip::tcp::socket(m_ios));
    m_acceptor.async_accept(*server_socket,boost::bind(&setpic_server::on_acceptor
                                                        ,this
                                                        ,boost::asio::placeholders::error
                                                        ,server_socket));
}

void setpic_server::on_read_done(const boost::system::error_code& err
                                ,size_t get_count
                                ,boost::shared_ptr<tcp_st> read_buffer
                                ,boost::shared_ptr<tcp_st> write_buffer
                                ,boost::shared_ptr<boost::asio::ip::tcp::socket> server_socket)
{
    if(err)
    {
        if(boost::asio::error::eof==err)
        {
            std::fprintf(stderr, "%s\n","client closed");
        }
        else
        {
            std::fprintf(stderr, "%s\n", boost::system::system_error(err).what());
        }
        return ;
    }

    operator_camera(read_buffer,write_buffer);
    server_socket->async_write_some(boost::asio::buffer(write_buffer.get(),sizeof(tcp_st))
                                ,boost::bind(&setpic_server::on_write_done
                                                        ,this
                                                        ,boost::asio::placeholders::error
                                                        ,boost::asio::placeholders::bytes_transferred
                                                        ,read_buffer
                                                        ,write_buffer
                                                        ,server_socket)
                                );

}

void setpic_server::on_write_done(const boost::system::error_code& err
                                    ,size_t write_count
                                    ,boost::shared_ptr<tcp_st> read_buffer
                                    ,boost::shared_ptr<tcp_st> write_buffer
                                    ,boost::shared_ptr<boost::asio::ip::tcp::socket> server_socket)

{
    if(err)
    {
        std::fprintf(stderr, "error %s\n",boost::system::system_error(err).what());
        return ;
    }
    if(CMD_FOCUS_VALUE==read_buffer->id)
    {
        setpic_server::operator_camera(read_buffer,write_buffer);
        server_socket->async_write_some(boost::asio::buffer(write_buffer.get(),sizeof(tcp_st))
                                        ,boost::bind(&setpic_server::on_write_done
                                                        ,this
                                                        ,boost::asio::placeholders::error
                                                        ,boost::asio::placeholders::bytes_transferred
                                                        ,read_buffer
                                                        ,write_buffer
                                                        ,server_socket)
                                        );
    }
    else if(CMD_COUNTER_DATA == read_buffer->id)
    {
        setpic_server::operator_camera(read_buffer,write_buffer);
        server_socket->async_write_some(boost::asio::buffer(write_buffer.get(),sizeof(tcp_st))
                                        ,boost::bind(&setpic_server::on_write_done
                                                        ,this
                                                        ,boost::asio::placeholders::error
                                                        ,boost::asio::placeholders::bytes_transferred
                                                        ,read_buffer
                                                        ,write_buffer
                                                        ,server_socket)
                                        );
    }
    else
    {
        server_socket->async_read_some(boost::asio::buffer(read_buffer.get(),sizeof(tcp_st))
                                        ,boost::bind(&setpic_server::on_read_done
                                            ,this
                                            ,boost::asio::placeholders::error
                                            ,boost::asio::placeholders::bytes_transferred
                                            ,read_buffer
                                            ,write_buffer
                                            ,server_socket)
                                        );
    }
}


void setpic_server::operator_camera(boost::shared_ptr<tcp_st> input,
                                    boost::shared_ptr<tcp_st> output)
{
    if(CMD_NET_TEST!=input->id)
    {
        std::fprintf(stderr, "get command id %d\n",input->id);
    }

    assert(0!=input && 0!=output);
    {
        switch(input->id)
        {
            case CMD_TRIGGER_MODE:
            {
                if(input->mode) //set
                {
                    int64_t nTrigger(0);
                    int32_t nCurrentTrigger(-1);
                    input->msg[sizeof(input->msg)-1]='\0';
                    std::string strTemp(input->msg);
                    nTrigger=strtol(strTemp.c_str(),0,10);
                    switch(nTrigger)
                    {
                        case 2:
                        {

                        }break;
                        case 1:
                        {
                            set_trigger_free();
                        }break;
                        case 0:
                        {
                            set_trigger_hardware();
                        }
                    }
                    char szTemp[32];
                    std::snprintf(szTemp,sizeof(szTemp),"%d",nCurrentTrigger);
                    std::memmove(output->msg,szTemp,sizeof(output->msg));
                }
                else
                {
                    //int32_t retval(0);
                    //int32_t mode(-1);
                    //if(0==(retval=pCamera->GetTriggerMode(mode)))
                    //{
                    //    std::fprintf(stderr, "%s\n",ebus_camera::get_error(retval));
                    //}
                    //char szTemp[32];
                    //std::snprintf(szTemp,sizeof(szTemp),"%d",mode);
                    //std::memmove(output->msg,szTemp,sizeof(output->msg));
                }
            }break;
            case CMD_EXPOSURE_TIME:
            {
                if(input->mode) //set
                {
                    std::string strTemp(output->msg);
                    int64_t lldExposure=strtol(strTemp.c_str(),0,10);
                    if(0==set_exposure_abs(lldExposure,lldExposure,lldExposure))
                    {
                        char szTemp[32];
                        std::snprintf(szTemp,sizeof(szTemp),"%lld",lldExposure);
                        std::memmove(output->msg,szTemp,sizeof(output->msg));
                    }
                }
                else
                {
                    int32_t retval(0);
                    long int exp_r(0),exp_g(0),exp_b(0);
                    if(0==get_exposure_abs(exp_r,exp_g,exp_b))
                    {
                        char szTemp[32];
                        std::snprintf(szTemp,sizeof(szTemp),"%lld",(long long int)exp_g);
                        std::memmove(output->msg,szTemp,sizeof(szTemp));
                    }
                    output->ret=retval;
                }
            }break;
            case CMD_GAIN:
            {
                if(input->mode) //set
                {
                    /*
                    std::string strTemp(output->msg);
                    int64_t lldGain=strtol(strTemp.c_str(),0,10);
                    if(0==pCamera->SetGain(lldGain))
                    {
                        char szTemp[32];
                        std::snprintf(szTemp,sizeof(szTemp),"%lld",lldGain);
                        std::memmove(output->msg,szTemp,sizeof(output->msg));
                    }*/
                }
                else
                {
                    /*
                    int64_t gain(0),min_gain(0),max_gain(0);
                    if(pCamera->GetGain(gain,min_gain,max_gain))
                    {
                        char szTemp[32];
                        std::snprintf(szTemp,sizeof(szTemp),"%lld",gain);
                        std::memmove(output->msg,szTemp,sizeof(szTemp));
                    }
                    */
                }
            }break;
            case CMD_WHITE_BALANCE:
            {
                if(input->mode) //set
                {
                    std::vector<int64_t> lRetval;
                    lRetval.reserve(3);
                    std::string strTemp(input->msg);
                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                    {
                        lRetval.push_back(strtol(std::string(*pos).c_str(),0,10));
                    }
                    uint32_t R(static_cast<uint32_t>(lRetval[0]));
                    uint32_t G(static_cast<uint32_t>(lRetval[1]));
                    uint32_t B(static_cast<uint32_t>(lRetval[2]));
                    R=(R<0)?0:((R>4096*4)?4096*4:R);
                    G=(G<0)?0:((G>4096*4)?4096*4:G);
                    B=(B<0)?0:((B>4096*4)?4096*4:B);
                    //pCamera->set_white_balance_rgb(R,G,B);
                    set_whitebalance(R/4096.0,G/4096.0,B/4096.0);
                    uint32_t _R(0),_G(0),_B(0);
                    //pCamera->get_white_balance_rgb(_R,_G,_B);
                    char szTemp[128];
                    std::snprintf(szTemp,sizeof(szTemp),"%u,%u,%u",_R,_G,_B);
                    std::memmove(output->msg,szTemp,sizeof(output->msg));
                }
                else
                {
                    uint32_t R(0),G(0),B(0);
                    //pCamera->get_white_balance_rgb(R,G,B);
                    if(true)
                    {
                        char szTemp[128];
                        std::snprintf(szTemp,sizeof(szTemp),"%u,%u,%u",R,G,B);
                        std::memmove(output->msg,szTemp,sizeof(szTemp));
                    }
                }
            }break;
            case CMD_COLOR_CORRECT:
            {
                if(input->mode) //set
                {

                }
                else
                {

                }
            }break;
            case CMD_CALI_CORRECT:
            {
                if(input->mode) //set
                {
                    std::vector<int> lRetval;
                    lRetval.reserve(1);
                    std::string strTemp(input->msg);
                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                    {
                        lRetval.push_back(atoi(std::string(*pos).c_str()));
                    }
                    int Flag(static_cast<int>(lRetval[0]));
                    if(0==Flag)
                    {
                        set_save_image(correct,1);
                        //pCamera->set_save_correct(1);//white
                    }
                    else
                    {
                        set_save_image(correct,0);
                        //pCamera->set_save_correct(0);//black
                    }
                }
                else
                {

                }
            }break;
            case CMD_DISTORTION:
            {
                if(input->mode) //set
                {
                    std::fprintf(stderr,"invalid id for set value,try get");
                }
                else
                {

                }
            }break;
            case CMD_FOCUS:
            {
                if(input->mode) //set
                {
                    std::fprintf(stderr,"invalid id for set value,try get");
                }
                else
                {

                }
            }break;
            case CMD_FOR_INIT_VALUE:
            {
                if(input->mode) //set
                {
                        std::vector<int64_t> lRetval;
                        lRetval.reserve(12);
                        std::string strTemp(input->msg);
                        boost::escaped_list_separator<char> sep;
                        boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                        for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                        {
                            lRetval.push_back(strtol(std::string(*pos).c_str(),0,10));
                        }

                        int64_t _trigger=static_cast<int64_t>(lRetval[2]);
                        int64_t _exposure=(lRetval[4]);
                        int64_t _gain=(lRetval[7]);
                        uint32_t _R=static_cast<uint32_t>(lRetval[9]);
                        uint32_t _G=static_cast<uint32_t>(lRetval[10]);
                        uint32_t _B=static_cast<uint32_t>(lRetval[11]);
                        int32_t _P1X=static_cast<int32_t>(lRetval[12]);
                        int32_t _P1Y=static_cast<int32_t>(lRetval[13]);
                        int32_t _P1W=static_cast<int32_t>(lRetval[14]);
                        int32_t _P1H=static_cast<int32_t>(lRetval[15]);
                        int32_t _P2X=static_cast<int32_t>(lRetval[16]);
                        int32_t _P2Y=static_cast<int32_t>(lRetval[17]);
                        int32_t _P2W=static_cast<int32_t>(lRetval[18]);
                        int32_t _P2H=static_cast<int32_t>(lRetval[19]);
                        int32_t _P3X=static_cast<int32_t>(lRetval[20]);
                        int32_t _P3Y=static_cast<int32_t>(lRetval[21]);
                        int32_t _P3W=static_cast<int32_t>(lRetval[22]);
                        int32_t _P3H=static_cast<int32_t>(lRetval[23]);
                        switch(_trigger)
                        {
                            case 2://hardware
                            {
                                set_trigger_hardware(); //error,no software support
                                //pCamera->TriggerSoftWare();
                            }break;
                            case 1:
                            {
                                set_trigger_free();
                                //pCamera->TriggerFree();
                            }break;
                            case 0:
                            {
                                set_trigger_hardware();
                                //pCamera->TriggerHardWare();
                            }break;
                            default:
                            break;
                        }
                        //set exposure
                        //pCamera->SetExposureTimeAbs(_exposure,_exposure,_exposure);
                        set_exposure_abs(_exposure,_exposure,_exposure);
                        //set gain
                        //pCamera->SetGain(_gain);
                        //set white balance
                        //pCamera->set_white_balance_rgb(_R,_G,_B);
                        set_whitebalance(_R/4096.0,_G/4096.0,_B/4096.0);
                        output->ret=0;
                }
                else
                {
                    int32_t retval(0);
                    uint32_t R(0),G(0),B(0);
                    int64_t lldGain(0),lldGainMin(0),lldGainMax(0);
                    int64_t lldExposure(0),lldExposureMin(0),lldExposureMax(0);
                    int64_t lldWidth(0),lldWidthMin(0),lldWidthMax(0);
                    int64_t lldHeight(0),lldHeightMin(0),lldHeightMax(0);
                    int32_t nTrigger(-1);
                    /*
                    pCamera->GetTriggerMode(nTrigger);
                    pCamera->get_white_balance_rgb(R,G,B);
                    if(0!=(retval=pCamera->GetGain(lldGain,lldGainMin,lldGainMax)))
                    {
                        std::fprintf(stderr, "%s\n",ebus_camera::get_error(retval));
                    }
                    if(0!=(retval=pCamera->GetExposureTimeAbs(lldExposure,lldExposureMin,lldExposureMax)))
                    {
                        std::fprintf(stderr, "%s\n",ebus_camera::get_error(retval));
                    }
                    if(0!=(retval=pCamera->GetWidth(lldWidth,lldWidthMin,lldWidthMax)))
                    {
                        std::fprintf(stderr, "%s\n",ebus_camera::get_error(retval));
                    }
                    if(0!=(retval=pCamera->GetHeight(lldHeight,lldHeightMin,lldHeightMax)))
                    {
                        std::fprintf(stderr, "%s\n",ebus_camera::get_error(retval));
                    }
                    char szTemp[256];
                    std::snprintf(szTemp,sizeof(szTemp),"%lld,%lld,%d,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld",
                                lldWidth,lldHeight,nTrigger,lldExposureMin,lldExposure,lldExposureMax,
                                lldGainMin,lldGain,lldGainMax,(int64_t)R,(int64_t)G,(int64_t)B);
                    std::memmove(output->msg,szTemp,sizeof(output->msg));
                    */
                }
            }break;
            case CMD_CALIBRATE_IMAGE:
            {
                if(input->mode) //set
                {
                    set_save_image(calibrate_board);
                    //pCamera->set_save_board();
                    std::fprintf(stderr, "%s\n","set_save_image:set board");
                }
                else
                {
                    set_save_image(calibrate_board);
                    std::fprintf(stderr, "%s\n","set_save_image:get board");
                }
            }break;
            case CMD_LEFT_RECT:
            {
                if(input->mode) //set
                {
                        std::vector<int64_t> lRetval;
                        lRetval.reserve(4);
                        std::string strTemp(input->msg);
                        boost::escaped_list_separator<char> sep;
                        boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                        for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                        {
                            lRetval.push_back(atoi(std::string(*pos).c_str()));
                        }
                        int32_t P1X(static_cast<int32_t>(lRetval[0]));
                        int32_t P1Y(static_cast<int32_t>(lRetval[1]));
                        int32_t P2X(static_cast<int32_t>(lRetval[2]));
                        int32_t P2Y(static_cast<int32_t>(lRetval[3]));
                }
                else
                {

                }
            }break;
            case CMD_MIDDLE_RECT:
            {
                if(input->mode) //set
                {
                    std::vector<int32_t> lRetval;
                    lRetval.reserve(4);
                    std::string strTemp(input->msg);
                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                    {
                        lRetval.push_back(atoi(std::string(*pos).c_str()));
                    }
                    int32_t P1X(static_cast<int32_t>(lRetval[0]));
                    int32_t P1Y(static_cast<int32_t>(lRetval[1]));
                    int32_t P2X(static_cast<int32_t>(lRetval[2]));
                    int32_t P2Y(static_cast<int32_t>(lRetval[3]));
                }
                else
                {

                }
            }break;
            case CMD_RIGHT_RECT:
            {
                if(input->mode) //set
                {
                    std::vector<int32_t> lRetval;
                    lRetval.reserve(4);
                    std::string strTemp(input->msg);
                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                    {
                        lRetval.push_back(atoi(std::string(*pos).c_str()));
                    }
                    int32_t P1X(static_cast<int32_t>(lRetval[0]));
                    int32_t P1Y(static_cast<int32_t>(lRetval[1]));
                    int32_t P2X(static_cast<int32_t>(lRetval[2]));
                    int32_t P2Y(static_cast<int32_t>(lRetval[3]));
                }
                else
                {

                }
            }break;
            case CMD_FOCUS_VALUE:
            {
                if(input->mode) //set
                {
                    //not set
                }
                else
                {
                    double _val(0);
                    //----------------------pStreamObj->get_focus(_val);
                    char szRet[32];
                    std::snprintf(szRet,sizeof(szRet),"%lf,",_val);
                    std::memmove(output->msg,szRet,sizeof(szRet));
                    output->ret=0;
                    usleep(50000);
                }
            }break;
            case CMD_NET_TEST:
            {
                if(input->mode) //set
                {

                }
                else
                {

                }
            }break;
            case CMD_SEQ_START:
            {
                if(input->mode) //set
                {
                    std::vector<int64_t> lRetval;
                    lRetval.reserve(1);
                    std::string strTemp(input->msg);
                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                    {
                        lRetval.push_back(atoi(std::string(*pos).c_str()));
                    }
                    int64_t Flag(static_cast<int64_t>(lRetval[0]));

                    //pCamera->set_seq_index((int64_t)Flag);
                }
                else
                {

                }
            }break;
            case CMD_GET_IMGAE_WIDTH_HEIGHT:
            {
                if(input->mode) //set
                {

                }
                else
                {
                        /*
                        int32_t retval(0);
                        int64_t lldWidth(0),lldWidthMin(0),lldWidthMax(0);
                        int64_t lldHeight(0),lldHeightMin(0),lldHeightMax(0);
                        if(0!=(retval=(pCamera->GetWidth(lldWidth,lldWidthMin,lldWidthMax))))
                        {
                            std::fprintf(stderr, "%s\n",ebus_camera::get_error(retval));
                        }
                        if(0!=(retval=(pCamera->GetHeight(lldHeight,lldHeightMin,lldHeightMax))))
                        {
                            std::fprintf(stderr, "%s\n",ebus_camera::get_error(retval));
                        }
                        char szTemp[32];
                        std::snprintf(szTemp,sizeof(szTemp),"%lld,%lld",lldWidth,lldHeight);
                        std::memmove(output->msg,szTemp,sizeof(szTemp));
                        */
                }
            }break;
            case CMD_RESET_CORECT_RECT:
            {
                if(input->mode) //set
                {
                    //-------------pStreamObj->set_rect(0,0,0,0,0);
                    //-------------pStreamObj->set_rect(1,0,0,0,0);
                    //-------------pStreamObj->set_rect(2,0,0,0,0);
                }
                else
                {

                }
            }break;
            case CMD_GET_VERSION_STRING:
            {
                if(input->mode) //set
                {
                    //use get
                }
                else
                {

                }
            }break;
            case CMD_CLOSE_SETPIC:
            {
                if(input->mode) //set
                {
                    global_is_exit=true;
                    stop();
                }
                else
                {

                }
            }break;
            case CMD_MAKE_CALIBRATE_XML:
            {
                if(input->mode) //set
                {

                }
                else
                {

                }
            }break;
            case CMD_SEQ_STOP:
            {
                if(input->mode) //set
                {
                    //pCamera->set_seq_index(0);
                }
                else
                {

                }
            }break;
            case CMD_SOFTTRIGGER_ONCE:
            {
                if(input->mode) //set
                {

                }
                else
                {

                }
            }break;
            case CMD_IMAGE_PROCESS_SET:
            {
                if(input->mode) //set
                {

                }
                else
                {

                }
            }break;
            case CMD_COUNTER_SWITCH:
            {
                if(input->mode) //set
                {

                }
                else
                {

                }
            }break;
            case CMD_COUNTER_DATA:
            {
                if(input->mode) //set
                {

                }
                else
                {

                }
            }break;
            case CMD_WHITE_BALANCE_EXPOSURE_RGB_GAIN_MRB:
            {
                if(input->mode)
                {
                    std::vector<unsigned int> lRetval;
                    lRetval.reserve(6);
                    std::string strTemp(input->msg);
                    boost::escaped_list_separator<char> sep;
                    boost::tokenizer< boost::escaped_list_separator<char> > tok(strTemp,sep);
                    for(BOOST_AUTO(pos,tok.begin());pos!=tok.end();++pos)// lRetval.push_back(boost::filesystem::path(*pos).string())
                    {
                        lRetval.push_back(strtol(std::string(*pos).c_str(),0,10));
                    }
                    if(lRetval.size()>=3)
                    {
                        unsigned int lld_wb_r=static_cast<unsigned int>(lRetval[0]);
                        unsigned int lld_wb_g=static_cast<unsigned int>(lRetval[1]);
                        unsigned int lld_wb_b=static_cast<unsigned int>(lRetval[2]);
                        std::fprintf(stderr, "<line> %d <file> %s \n",__LINE__,__FILE__);
                        set_whitebalance(lld_wb_r/4096.0,lld_wb_b/4096.0,lld_wb_b/4096.0);
                        // pCamera->set_white_balance_rgb(lld_wb_r,lld_wb_g,lld_wb_b);
                    }
                    if(lRetval.size()>=6)
                    {
                        unsigned int lld_gain_r=static_cast<unsigned int>(lRetval[3]);
                        unsigned int lld_gain_g=static_cast<unsigned int>(lRetval[4]);
                        unsigned int lld_gain_b=static_cast<unsigned int>(lRetval[5]);
                        std::fprintf(stderr, "<line> %d <file> %s \n",__LINE__,__FILE__);
                        // pCamera->SetExposureTimeAbs(lld_gain_r,lld_gain_g,lld_gain_b);
                        set_exposure_abs(lld_gain_r,lld_gain_g,lld_gain_b);
                    }
                    output->ret=0;
                }
                else
                {

                }
            }break;
            case CMD_BALIBRATE_IMAGE_AND_XML:
            {
                if(input->mode)
                {
                    //pCamera->set_save_board();
                }
                else
                {
                    //pCamera->set_save_board();
                }
                output->ret=0;
            }break;
            default:
            {}break;
        }
    }

}


void setpic_server::run()
{
    m_ios.run();
}

void setpic_server::stop()
{
    m_ios.stop();
}


void setpic_tcp_socket(void)
{
    if(global_is_tcp_run) return;
    setpic_server::tcp_mode=CAMERA_MODE;
    setpic_server setpic_tcp(0);
    global_is_tcp_run=true;
    //setpic_tcp.run();
    setpic_server::run();
    global_is_tcp_run=false;
}
