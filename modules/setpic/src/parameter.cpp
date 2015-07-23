#include "parameter.h"


#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <map>

#include <boost/program_options.hpp>
#include <boost/typeof/typeof.hpp>


class Parse
{
public:
    Parse(const int& argc,char** argv);
    virtual ~Parse(void) throw();
private:
    explicit Parse(void* config);

public:
    bool CheckHelp(void) const;
    bool CheckIsDefault(const char* option) const;
    bool PrintHelp(void) const;
    bool GetValueInt(const char* option,int& retval) const;
    bool GetValueString(const char*,std::string& retval) const;
    bool GetValueUnrecognized(std::vector<std::string>& retval)const;

private:
    boost::program_options::options_description             CmdOption;          //Command line Description
    boost::program_options::variables_map                   CmdMap;             //Map of Command Line which is parsed
    boost::program_options::command_line_parser             CmdParser;
    std::vector<std::string>                                vUnrecognized;
};



Parse::Parse(const int& argc,char** argv)
        :CmdOption("\nSetPic VERSION V3.\nCommand Line Options\n YOUR CAN ALSO CONFIG BY CONFIG FILE: <config.lua>\n")
        ,CmdParser(argc,argv)
{
    CmdOption.add_options()
        ("help,h","list all the valid arguments")
        ("count,c",boost::program_options::value<int>()->default_value(0)->implicit_value(0),"the count of image you want to put into shared memory of each class\n")
        ("train,t",boost::program_options::value<std::string>()->default_value(std::string("train.conf"))->implicit_value(std::string("train.conf")),"path of train-config file in train mode,default and implicit value: train.conf\n")
        ("run,r",boost::program_options::value<std::string>()->default_value(std::string("./IMG"))->implicit_value(std::string("./../IMG")),"path of image,can be a directory or a file list the image path,auto analysis\n")
        ("xpressive,x",boost::program_options::value<std::string>()->default_value(std::string("^\\d*.bmp$"))->implicit_value(std::string("^\\d*.bmp$")),"the xpressive of image filename,attention ,not path\n")
        ("section,s",boost::program_options::value<int>()->default_value(2)->implicit_value(2),"the number of share memory ,at least: 2 \n")
        ("left,l",boost::program_options::value<int>()->default_value(0)->implicit_value(0),"left shelfer\n")
        ("mode,m",boost::program_options::value<int>()->default_value(0)->implicit_value(0),"0 SetPic for Image,1 For Area Camera Mode , 2 for Linear Camera other,else Discard.\n")
        ("write,w",boost::program_options::value<std::string>()->default_value(std::string("./"))->implicit_value(std::string("./")),"Path To Save The Image In Camera Mode .\n")
        ("gain,g",boost::program_options::value<int>()->default_value(0)->implicit_value(0),"the Camera Gain ,Disable In Image Mode.\n")
        ("gainR,R",boost::program_options::value<int>()->default_value(4096)->implicit_value(4096),"the Camera WhiteBalance R Value ,Disable In Image Mode.\n")
        ("gainG,G",boost::program_options::value<int>()->default_value(4096)->implicit_value(4096),"the Camera WhiteBalance G Value ,Disable In Image Mode.\n")
        ("gainB,B",boost::program_options::value<int>()->default_value(4096)->implicit_value(4096),"the Camera WhiteBalance B Value ,Disable In Image Mode.\n")
        ("exposuretimeabs,E",boost::program_options::value<int>()->default_value(1200)->implicit_value(1200),"the Camera ExposureTimeAbs ,Disable In Image Mode. \n")
        ("modbus_ip",boost::program_options::value<std::string>()->default_value(std::string("127.0.0.1"))->implicit_value(std::string("127.0.0.1")),"ModBus Server IP Address.\n")
        ("modbus_port",boost::program_options::value<int>()->default_value(9001)->implicit_value(9001),"ModBus Server Port.\n")
        ("times,i",boost::program_options::value<int>()->default_value(-1)->implicit_value(-1),"running times\n")
        ("camera_config",boost::program_options::value<std::string>()->default_value(std::string("camera.pvcfg"))->implicit_value(std::string("camera.pvcfg")),"Gevplayer Config File.\n");
    BOOST_AUTO(parser_opt,CmdParser.options(CmdOption).allow_unregistered().run());
    boost::program_options::store(parser_opt,CmdMap);
    boost::program_options::notify(CmdMap);
    vUnrecognized=boost::program_options::collect_unrecognized(parser_opt.options,boost::program_options::include_positional);

}

Parse::~Parse() throw()
{

}

bool Parse::CheckHelp() const
{
    return (this->CmdMap.count("help") >=1)?true:false;
}

bool Parse::CheckIsDefault(const char* option) const
{
    return this->CmdMap[option].defaulted()?true:false;
}

bool Parse::PrintHelp() const
{
    this->CmdOption.print(std::cout);
    return true;
}

bool Parse::GetValueInt(const char* option,int& retval) const
{
    bool Bretval(false);
    try
    {
        if(0==&retval)
        {
            return false;
        }
        retval=this->CmdMap[option].as<int>();
        Bretval=true;
    }
    catch(std::exception& e)
    {
        std::fprintf(stderr, "[**** ERROR   ]:%s\n",e.what());
        Bretval=false;
    }
    return Bretval;
}

bool Parse::GetValueString(const char* option,std::string& retval) const
{
    bool Bretval(false);
    try
    {
        if(0==&retval)
        {
            return false;
        }
        retval=this->CmdMap[option].as<std::string>();
        Bretval=true;
    }
    catch(std::exception& e)
    {
        std::fprintf(stderr, "[**** ERROR   ]:%s\n",e.what());
        Bretval=false;
    }
    return Bretval;
}

bool Parse::GetValueUnrecognized(std::vector<std::string>& retval) const
{
    bool Bretval(false);
    try
    {
        if(0==&retval)
        {
            return false;
        }
        retval=this->vUnrecognized;
        Bretval=true;
    }
    catch(std::exception& e)
    {
        std::fprintf(stderr, "[**** ERROR   ]:%s\n",e.what());
        Bretval=false;
    }
    return Bretval;
}

/***********************************************************\
*                                                           *
*                                                           *
*      INPUT PARAMETER INTERFACE                            *
*                                                           *
*                                                           *
\***********************************************************/
static bool __g_is_inited(false);

static Parse* param;

void __set_parameter_flag(bool is_inited)
{
    __g_is_inited=is_inited;
}

bool __get_parameter_flag(void)
{
    return __g_is_inited;
}


#define __CHECK_PARAMETER_INITED \
        if(false==__get_parameter_flag()) \
        { \
            std::fprintf(stderr, "[**** ERROR   ] %s\n","parameter not inited."); \
            return false; \
        }

bool init_parameter(int argc,char** argv)
{
    if(true==__get_parameter_flag()) return true;
    param=new Parse(argc,argv);
    __set_parameter_flag(true);
    return __get_parameter_flag();
}


bool uninit_parameter(void)
{
    if(false==__get_parameter_flag) return false;
    if(param)
    {
        delete param;
        param=NULL;
    }
    __set_parameter_flag(false);
    return true;
}

bool check_help(void)
{
    __CHECK_PARAMETER_INITED;
    return param->CheckHelp();
}

bool check_is_default(const char* option)
{
    __CHECK_PARAMETER_INITED;
    return param->CheckIsDefault(option);
}

void print_help(void)
{
    param->PrintHelp();
}

bool get_parameter_int(const char* option,int& retval)
{
    __CHECK_PARAMETER_INITED;
    return param->GetValueInt(option,retval);
}

bool get_parameter_string(const char* option,std::string& retval)
{
    __CHECK_PARAMETER_INITED;
    return param->GetValueString(option,retval);
}

bool get_parameter_unrecongnized(std::vector<std::string>& retval)
{
    __CHECK_PARAMETER_INITED;
    return param->GetValueUnrecognized(retval);
}

