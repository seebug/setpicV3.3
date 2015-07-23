#include "lscript.h"

#include <cstdlib>
#include <vector>
#include <algorithm>


void print_list(const config_type& config_value)
{

    switch(config_value.type_name)
    {
        case unknown_key_word:
        {
            std::fprintf(stderr, "[list]category:? type: %s key: %s\t\t value: %s\n","unknown","unknown","unknown");
        }break;

        case string_key_word:
        {
            std::fprintf(stderr, "[list]category:%d type: %s key: %s\t\t value: %s\n",(int)config_value.category,"string",config_value.key_name.c_str(),config_value.string_value.c_str());
        }break;
        case integer_key_word:
        {
            std::fprintf(stderr, "[list]category:%d type: %s key: %s\t\t value: %d\n",config_value.category,"int32_t",config_value.key_name.c_str(),(int32_t)config_value.int32_value);
        }break;
        case long_integer_key_word:
        {
            std::fprintf(stderr, "[list]category:%d type: %s key: %s\t\t value: %lld\n",config_value.category,"int64_t",config_value.key_name.c_str(),(int64_t)config_value.int64_value);
        }break;
        case double_key_word:
        {
            std::fprintf(stderr, "[list]category:%d type: %s key: %s\t\t value: %lf\n",config_value.category,"double",config_value.key_name.c_str(),(double)config_value.double_value);
        }break;
        case command_key_word:
        {
            std::fprintf(stderr, "[list]category:%d type: %s key: %s\t\t value: unknown\n",config_value.category,"command",config_value.key_name.c_str());
        }break;
    }

    return ;
}



int main(int agrc,char* argv[])
{
    char** lpcfg=NULL;
    char** lptable=NULL;
    if(agrc<3)
    {
        std::fprintf(stderr, "%s\n","parameter error.");
        return EXIT_FAILURE;
    }
    lpcfg=&argv[1];
    lptable=&argv[2];
    std::fprintf(stderr, "agrv[1]%s\n",*lpcfg);
    std::fprintf(stderr, "argv[2]%s\n",*lptable);
    lua_script script(*lpcfg);
    std::fprintf(stderr, "%s\n","create instance.");
    if(!script.valid())
    {
        std::fprintf(stderr, "%s\n","script valid");
        return EXIT_FAILURE;
    }
    std::vector<config_type> config;

    script.read_table_item(*lptable,config);
    std::for_each(config.begin(),config.end(),print_list);
    std::fprintf(stderr, "%s\n", "press any key go on ...");
    getchar();
    return 0;
}
