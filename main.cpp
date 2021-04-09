#include "opt.h"
#include "multicast.h"

#include <signal.h>

void help();

void procExit(int signo);

int main(int argc,char **argv)
{
    signal(SIGINT,procExit);
    signal(SIGTERM,procExit);

    std::string m = "send";
    std::string addr = "224.1.0.104:1235,127.0.0.1";

    Option opt(argc,argv,help);
    opt.addOpt(m,"m:");
    opt.addOpt(addr,"a:");
    opt.init();

    std::cout<< "method:" << m<<std::endl;
    std::cout<< "addr:" << addr <<std::endl;

    return 0;
}


void help()
{
    std::cout<<"help"<<std::endl;
}

void procExit(int signo)
{
    std::cout<<"multicast recv signal:"<<signo<<" to exit"<<std::endl;
}

void send()
{

}

void recv()
{

}