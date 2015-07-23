#include "setpic.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <signal.h>

#include <dlfcn.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "parameter.h"
#include "rtmemory.h"
#include "modbusnet.h"
#include "server.h"
#include "setpic.h"
#include "lscript.h"
#include "callback.h"
#include "vcamera.h"
#ifndef SET_VERSION_COMPILE_OS
#define SET_VERSION_COMPILE_OS "unknown"
#endif

#ifndef SET_VERSION_COMPILE_HOST
#define SET_VERSION_COMPILE_HOST "unknown"
#endif

#ifndef SET_VERSION_COMPLIE_KERNEL
#define SET_VERSION_COMPLIE_KERNEL "unknown"
#endif


bool global_is_exit(false);

std::string GetVersionString()
{
    //GUN/Linux-Shmily-PC-2.6.32-5-686
    char szVer[128];
    std::memset(szVer,0x00,sizeof(szVer));
    std::string _compile_date(__DATE__);
    std::string _compile_time(__TIME__);
    std::string compile_date(boost::replace_all_copy(_compile_date," ","-"));
    std::string compile_time(boost::replace_all_copy(_compile_time,":",""));
    std::string str_compile_date_time(boost::gregorian::to_iso_string(boost::gregorian::from_us_string(compile_date))+compile_time);
    std::sprintf(szVer,"%s-%s-%s Version=%d.%d.%d Build:%d TS:%s",SET_VERSION_COMPILE_OS,SET_VERSION_COMPILE_HOST,SET_VERSION_COMPLIE_KERNEL,_V_CAM_MAJOR_VERSION_,_V_CAM_MINOR_VERSION_,_V_CAM_REVISION_VERSION,_V_CAM_BUILD_VERSION,str_compile_date_time.c_str());
    std::string ret(szVer);
    return ret;
}

void on_signal_sigint(int sign_no)
{
    global_is_exit=true;
    usleep(20000);
    signal(SIGINT,SIG_DFL);
}

void on_signal_sigquit(int sign_no)
{
    global_is_exit=true;
}

void on_signal_sigterm(int sign_no)
{
    global_is_exit=true;
}


bool check_info()
{
    bool b_well_exit(true);
    lua_script config_script("./camera.luac");
    bool bchecksystemdog(true),bchecksysteminfo(true),bencryption(true),bencryption_exit(true);
    if(0==config_script.get<bool>("checksystemdog",&bencryption))
    {
        bchecksystemdog=bencryption;
        std::fprintf(stderr, "[checksystemdog] %d\n", (int)bchecksystemdog);
    }
    if(0==config_script.get<bool>("checksysteminfo",&bencryption))
    {
        bchecksysteminfo=bencryption;
        std::fprintf(stderr, "[bchecksysteminfo] %d\n",(int)bchecksystemdog);
    }
    if(0==config_script.get<bool>("exit_noencryption",&bencryption))
    {
        bencryption_exit=bencryption;
        std::fprintf(stderr, "[bencryption_exit] %d\n", (int)bchecksystemdog);
    }
    typedef void (*checksysteminfo_func)(void);
    typedef void (*checksystemdog_func)(void);

    void* handle=dlopen("./libencryption.so",RTLD_LAZY);
    if(handle)
    {
        if(bchecksysteminfo)
        {
            checksysteminfo_func init_info=(checksysteminfo_func)(uintptr_t)dlsym(handle,"systeminfocheck");
            if(!init_info)
            {
                dlclose(handle);
                std::fprintf(stderr, "%s\n","can not init your system information !");
                b_well_exit=false;
                goto label_exit;
            }
            init_info();
        }

        if(bchecksystemdog)
        {
            checksystemdog_func monitor=(checksysteminfo_func)(uintptr_t)dlsym(handle,"systemdogcheck");
            if(!monitor)
            {
                dlclose(handle);
                std::fprintf(stderr, "%s\n","can not check your system dog");
                b_well_exit=false;
                goto label_exit;
            }
            monitor();
        }
        dlclose(handle);
    }
    else
    {
        std::fprintf(stderr, "%s\n",dlerror());
        if(bencryption_exit)
        {
            b_well_exit=false;
            goto label_exit;
        }
    }
label_exit:
    return b_well_exit;
}


bool pre_set_camera()
{

}



int main(int argc,char* argv[])
{
    std::fprintf(stderr, "%s\n",GetVersionString().c_str());
    signal(SIGINT,on_signal_sigint);
    signal(SIGQUIT,on_signal_sigquit);
    signal(SIGTERM,on_signal_sigterm);
    bool b_well_exit(true);
    if(!init_parameter(argc,argv)) return EXIT_FAILURE;
    if(check_help())
    {
        print_help();
        b_well_exit=false;
        goto label_exit;;
    }

    if(!check_info()) exit(EXIT_FAILURE);

    {
        int nSection(2);
        if(!get_parameter_int("section",nSection)) nSection=2;
        nSection<2?2:nSection;
        init_dir_runtime(nSection,"virtual");
        set_shm(vShmKey,nSection);
        std::string pre_log_dir("./runtime/log/");
        std::string log_dir(boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()));
        boost::filesystem::path stream_log_dir(pre_log_dir+log_dir);
        if(!boost::filesystem::exists(stream_log_dir))
        {
            boost::filesystem::create_directories(stream_log_dir);
        }

        std::string stream_log_dir_str(stream_log_dir.string());
        std::string _stderr(stream_log_dir_str+"/./stderr.txt");
        std::string _stdout(stream_log_dir_str+"/./stdout.txt");
        //std::FILE* __stderr(freopen(_stderr.c_str(),"w",stderr));
        //std::FILE* __stdout(freopen(_stdout.c_str(),"w",stdout));

        bool is_tcp_server_run(false);


        std::string strIP("127.0.0.1");
        int nPort(9600);



        //boost::asio::io_service     mod_ios;
        //ModBusNet* modbus=new ModBusNet(mod_ios,strIP,nPort);
        //boost::asio::io_service::work _work(mod_ios);
        //boost::thread thread_modbus(boost::bind(&boost::asio::io_service::run,&mod_ios));

        if(!is_tcp_server_run)
        {
            boost::thread thread_tcp(setpic_tcp_socket);
            is_tcp_server_run=true;
        }

        int nMode(0);
        if(!get_parameter_int("mode",nMode)) nMode=0;
        switch(nMode)
        {
            case 1:
            {

                std::string camera_config_file("");
                if(true!=get_parameter_string("camera_config",camera_config_file))
                {
                    std::fprintf(stderr, "%s\n","cannot get config parameter:\"camera_config\" ");
                    goto label_exit;
                }

                if(check_is_default("camera_config"))
                {
                    camera_config_file="camera.pvcfg";
                }

                usleep(400000);

                if(0!=load_config(camera_config_file.c_str()))
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","cannot load config");
                    goto label_exit_camera;
                }

                if(0!=restore_config())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","cannot restore config");
                    goto label_exit_camera;
                }

                if(0!=open())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","cannot open camera");
                    goto label_exit_camera;
                }

                if(0!=set_buffer())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","set buffer error");
                    goto label_exit_camera;
                }

                if(0!=set_callback(callback_func))
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","set callback error");
                    goto label_exit_camera;
                }
                /*
                if(0!=set_trigger_hardware())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","set trigger free");
                    goto label_exit_camera;
                }
                */
                if(0!=grab())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","error grab");
                    goto label_exit_camera;
                }
                else
                {
                    std::fprintf(stderr, "%s\n","grab suceee");
                }
                while(is_tcp_server_run && (false==global_is_exit) )
                {
                    // create shared memory
                    //if()
                    std::fprintf(stderr, "%s\n","loop");
                    sleep(2);
                }

                if(0!=stop())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","stop error");
                    goto label_exit_camera;
                }

                if(0!=unset_buffer())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","unset buffer");
                    goto label_exit_camera;
                }

                if(0!=close())
                {
                    b_well_exit=false;
                    std::fprintf(stderr, "%s\n","close error");
                    goto label_exit_camera;
                }

label_exit_camera:
                unload_config();

            }break;
            case 0:
            {
                image_mode_st param;
                get_parameter_string("train",param.train);
                get_parameter_string("run",param.run);
                get_parameter_string("xpressive",param.xpressive);
                get_parameter_int("count",param.count);
                get_parameter_int("section",param.section);
                get_parameter_int("left",param.left);
                get_parameter_int("times",param.times);

                _load_config(&param);
                _grab();
                _unload_config();
            }break;
            default:break;
        }

        //if(modbus)
        //{
        //    delete modbus;
        //    modbus=NULL;
        //}
        /*
        if(__stdout)
        {
            fclose(__stdout);
            __stdout=NULL;
        }

        if(__stderr)
        {
            fclose(__stderr);
            __stderr=NULL;
        }
        */
    }
label_exit:
    uninit_parameter();
    if(b_well_exit) return EXIT_SUCCESS;
    else return EXIT_FAILURE;
}