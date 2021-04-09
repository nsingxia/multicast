#include <unistd.h>
#include <map>
#include <string>
#include <functional>
#include <iostream>

#include <string.h>

using VARFUN = std::function<void(void)>;

class Option
{
public:
    Option(int argc,char **argv,VARFUN h = nullptr) : argc_(argc),argv_(argv),help_(h)
    {
        addHelp();
    }

    void addOpt(int &var,std::string name)
    {
        addMap(name,[&]{
            var = atoi(optarg);
        });
    }

    void addOpt(std::string &var,std::string name)
    {
        addMap(name,[&]{
            var = optarg;
        });
    }

    void addOpt(char *var,std::string name)
    {
        addMap(name,[=]{
            strcpy(var,optarg);
        });
    }

    void addOpt(float &var,std::string name)
    {
        addMap(name,[&]{
            var = atof(optarg);
        });
    }

    void addOpt(int64_t &var,std::string name)
    {
        addMap(name,[&]{
            var = atoll(optarg);
        });
    }

    void addOpt(bool &var,std::string name)
    {
        addMap(name,[&]{
            var = strcmp("true",optarg) == 0 ? true:false;
        });
    }

    void init()
    {
        char c = 0;
        while((c = getopt(argc_,argv_,optStr_.c_str())) != EOF)
        {
            auto it = funMap_.find(c);
            if(it != funMap_.end())
            {
                it->second();
            }
            else
            {
                if(help_ != nullptr)
                {
                    help_();
                    exit(0);
                }
            }
        }
    }


private:
    void addMap(std::string name,VARFUN fun)
    {
        if(!checkName(name))
        {
            std::cerr<<"check name failed, name:"<<name<<std::endl;
            return;
        }
        funMap_[name[0]] = fun;
        optStr_ += name;
    }

    bool checkName(std::string name)
    {
        if(name.empty() || name.size() > 2) return false;
        if(name.at(0) == ':') return false;
        if(name.size() == 2 && name[1] != ':') return false;

        return true;
    }

    void help()
    {
        if(help_ != nullptr)
        {
            help_();
        }
        else
        {
            std::cerr<<"invalid parameter"<<std::endl;
        }
        exit(0);
    }

    void addHelp()
    {
         addMap("h",[&]{
            help();
        });
    }

private:
    int argc_;
    char **argv_;

    std::map<char,VARFUN> funMap_;
    std::string optStr_;

    VARFUN help_ = nullptr;
};