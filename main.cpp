#include "opt.h"
#include "multicast.h"

void help();

int main(int argc,char **argv)
{
    std::string m;
    Option opt(argc,argv,help);
    opt.addOpt(m,"m:");
    opt.init();
    return 0;
}


void help()
{
    std::cout<<"help"<<std::endl;
}