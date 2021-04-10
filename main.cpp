#include "opt.h"
#include "multicast.h"

#include <signal.h>
#include <thread>

void help();

void procExit(int signo);

bool isStop = false;

void send(std::string addr);
void recv(std::string addr);

int main(int argc,char **argv)
{
    signal(SIGINT,procExit);
    signal(SIGTERM,procExit);

    std::string m = "send";
    std::string addr = "224.1.0.104:10021,172.18.0.1";

    Option opt(argc,argv,help);
    opt.addOpt(m,"m:");
    opt.addOpt(addr,"a:");
    opt.init();

    std::cout<< "method:" << m<<std::endl;
    std::cout<< "addr:" << addr <<std::endl;

    if(m == "send")
    {
        send(addr);
    }
    else
    {
        recv(addr);
    }

    return 0;
}


void help()
{
    std::cout<<"help"<<std::endl;
}

void procExit(int signo)
{
    std::cout<<"multicast recv signal:"<<signo<<" to exit"<<std::endl;
    isStop = true;
}

void send(std::string addr)
{
    std::cout<<"send is running"<<std::endl;
    PubMultiCast sender(addr);
    char buf[4096] = {0};
    int index = 0;
    int n  = 0;

    while(!isStop)
    {
        sprintf(buf,"send msg index:%d",index++);
        n = sender.SendMsg(buf,strlen(buf));
        std::cout<<"n:"<<n<<std::endl;
        sleep(1);
    }
}

void recv(std::string addr)
{
    std::cout<<"recv is running"<<std::endl;
    SubMultiCast recver(addr);
    char buf[4096] = {0};
    int n  = 0;

    while(!isStop)
    {
        n = recver.RecvMsg(buf,4096);
        if(n <= 0)
        {
            std::this_thread::yield();
            continue;
        }
        std::cout<<buf<<", n:"<<n<<std::endl;
    }
}