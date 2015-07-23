#include "rtmemory.h"

#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/shm.h>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/time_duration.hpp>

#include "lscript.h"

#define _SETPIC_IPC_FLAG_ 0x205

std::vector<long> vShmKey;

void init_dir_runtime(const int nSection,const char* sk)
{
    int32_t nC=((nSection<=2)?2:((nSection>=16)?16:nSection));
    /*
    std::string _sk(sk);
    if(_sk.empty())
    {
        _sk=std::string("memory");
    }
    */
    boost::filesystem::path strart_up_dir(AutoMove(boost::filesystem::initial_path()));
    char vir_dir[]="runtime";
    strart_up_dir.append(vir_dir,vir_dir+sizeof(vir_dir));
    if(!boost::filesystem::exists(strart_up_dir))
    {
        boost::filesystem::create_directories(strart_up_dir);
    }
    else
    {
        if(!boost::filesystem::is_directory(strart_up_dir))
        {
            boost::filesystem::path back_up_path(AutoMove(boost::filesystem::initial_path()));
            int32_t index(0);
            char buf[256];
        label_saft_move_flag:
            std::memset(buf,0x00,sizeof(buf));
            std::string strYYYYMMDD(AutoMove(boost::gregorian::to_iso_string(AutoMove(boost::posix_time::microsec_clock::local_time().date()))));
            std::sprintf(buf,"%s/%s_%s_%d",back_up_path.string().c_str(),"runtime_old",strYYYYMMDD.c_str(),index);
            buf[sizeof(buf)-1]='\0';

            if(boost::filesystem::exists(AutoMove(boost::filesystem::path(std::string(buf).c_str()))))
            {
                ++index;
                goto label_saft_move_flag;
            }
            boost::filesystem::rename(strart_up_dir,AutoMove(boost::filesystem::path(std::string(buf).c_str())));
            boost::filesystem::create_directories(strart_up_dir);
        }
    }
    char vir_dir_sub[256];
    std::memset(vir_dir_sub,0x00,sizeof(vir_dir_sub));
    std::sprintf(vir_dir_sub,"%s/%s",strart_up_dir.string().c_str(),"map");//,_sk.c_str());
    vir_dir_sub[sizeof(vir_dir_sub)-1]='\0';
    boost::filesystem::path dir_map(std::string(vir_dir_sub).c_str());
    if(!boost::filesystem::exists(dir_map))
    {
        boost::filesystem::create_directories(dir_map);
    }
    else
    {
        if(!boost::filesystem::is_directory(dir_map))
        {
            boost::filesystem::remove(dir_map);
            boost::filesystem::create_directories(dir_map);
        }
    }
    std::memset(vir_dir_sub,0x00,sizeof(vir_dir_sub));
    std::sprintf(vir_dir_sub,"%s/%s",strart_up_dir.string().c_str(),"log");
    vir_dir_sub[sizeof(vir_dir_sub)-1]='\0';
    boost::filesystem::path dir_log(std::string(vir_dir_sub).c_str());
    if(!boost::filesystem::exists(dir_log))
    {
        boost::filesystem::create_directories(dir_log);
    }
    else
    {
        if(!boost::filesystem::is_directory(dir_log))
        {
            boost::filesystem::remove(dir_log);
            boost::filesystem::create_directories(dir_log);
        }
    }
    std::memset(vir_dir_sub,0x00,sizeof(vir_dir_sub));
    std::sprintf(vir_dir_sub,"%s/%s",strart_up_dir.string().c_str(),"data");
    vir_dir_sub[sizeof(vir_dir_sub)-1]='\0';
    boost::filesystem::path dir_data(std::string(vir_dir_sub).c_str());
    if(!boost::filesystem::exists(dir_data))
    {
        boost::filesystem::create_directories(dir_data);
    }
    else
    {
        if(!boost::filesystem::is_directory(dir_data))
        {
            boost::filesystem::remove(dir_data);
            boost::filesystem::create_directories(dir_data);
        }
    }
    std::list<boost::filesystem::path> list_shm_lua_file_path;
    list_shm_lua_file_path.clear();
    int32_t n_max_shared_memory_count(nSection);
    vShmKey.reserve(nSection);
    while(n_max_shared_memory_count>0)
    {
        char _sz_file_path[256];
        std::memset(_sz_file_path,0x00,sizeof(_sz_file_path));
        std::sprintf(_sz_file_path,"%s/%d.lua",dir_map.string().c_str(),n_max_shared_memory_count-1);
        const boost::filesystem::path shminfo_path(std::string(_sz_file_path).c_str());
lable_recheck_shminfo_file_flag:
        if(boost::filesystem::exists(shminfo_path))
        {
            if(!boost::filesystem::is_directory(shminfo_path))
            {
                //bool b_over_write(false);
                uint32_t set_pic_base(_SETPIC_IPC_FLAG_);
                key_t real_key(ftok(shminfo_path.string().c_str(),set_pic_base));
                lua_script shm_info_script(shminfo_path.string().c_str());
                std::string _shm_file_path(".");
                //key_t shm_info_id(0);
                long shm_info_id(0);
                shm_info_script.get<std::string>("path",&_shm_file_path);
                shm_info_script.get<long>("id",&shm_info_id);

                if(0!=std::strcmp(_shm_file_path.c_str(),shminfo_path.string().c_str()) ||
                    real_key!=shm_info_id)
                {
                    std::fstream _file_shm_info(shminfo_path.c_str(),std::ios::out);
                    if(_file_shm_info)
                    {
                        try
                        {
                            std::string _shm_info_open_app( "#!/usr/bin/env lua\n");
                            _file_shm_info<<_shm_info_open_app<<std::endl;
                            std::string _shm_info_open_tip( "--[[\n\n"
                                                            "    ATTENTION : THIS FILE IS AUTO CREATED BY CAMERA SERVICE SETPIC V3.3 ,USING FOR SHARED MEMORY.\n"
                                                            "DO NOT CHANGE OR MOVE IT . ANY CHANGED IN THIS FILE MAY MAKE SHARED MEMORY NOT WORK .\n\n"
                                                            "                                           wangbin <wangbin@inovance.cn>\n"
                                                            "]]--\n");
                            _file_shm_info<<_shm_info_open_tip<<std::endl;
                            _file_shm_info<<"path   =\""<<shminfo_path.string()<<"\""<<std::endl;
                            _file_shm_info<<"id     ="<<real_key;
                            char _shm_info_delete_function[512];
                            std::memset(_shm_info_delete_function,0x00,sizeof(_shm_info_delete_function));
                            std::sprintf(_shm_info_delete_function,"\n\n--delete command: ipcrm -m shmid \n"
                                                               "function delete_this_shared_memory(...)\n"
                                                               "    io.write(tostring(_VERSION) .. tostring(\"\\t\") .. tostring(os.date()) .. tostring(\"\\n\"))\n"
                                                               "    if os.getenv [[HOME]] ~= tostring(\"/root\") then\n"
                                                               "        io.write(tostring(\"For Better Execute,Suggest You Login As \\\"root\\\"\\n\"))\n"
                                                               "    end\n"
                                                               "    os.execute(\"ipcrm -m %d\")\n"
                                                               "end\n"
                                                                    ,real_key);
                            _shm_info_delete_function[sizeof(_shm_info_delete_function)-1]='\0';
                            _file_shm_info<<std::string(_shm_info_delete_function)<<std::endl;
                        }
                        catch(std::exception& e)
                        {
                            std::fprintf(stderr, "%s\n",e.what());

                        }
                        _file_shm_info.close();
                    }
                }
                else
                {
                    vShmKey.push_back(real_key);
                    --n_max_shared_memory_count;
                }

            }
            else // exist ,but is dir
            {
                boost::filesystem::remove_all(shminfo_path);//recursive delete dir
                std::fstream _file_shm_info(shminfo_path.string().c_str(),std::ios::out);
                if(_file_shm_info)
                {
                    _file_shm_info.close();
                }
                goto lable_recheck_shminfo_file_flag;
            }
        }
        else //not exists
        {
            std::fstream _file_shm_info(shminfo_path.string().c_str(),std::ios::out);
            if(_file_shm_info)
            {
                _file_shm_info.close();
            }
            goto lable_recheck_shminfo_file_flag;
        }
    }
    std::reverse(vShmKey.begin(),vShmKey.end());
}

