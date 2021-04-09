#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <cassert>

#include <string>
#include <vector>
#include <iostream>


#define PARM_STR(x) std::cout<<#x":"<<x<<std::endl;


std::vector<std::string> split(std::string str,std::string sp)
{
    std::vector<std::string> res;

    int n = 0;
    while((n = str.find(sp)) >= 0)
    {
        res.emplace_back(str.substr(0,n));
        str = str.substr(n+sp.size());
    }

    if(!str.empty())
    {
        res.emplace_back(str);
    }

    return res;
}

class MultiCast {
public:
    MultiCast()
    {
        
    }

    void init(std::string addr)
    {
        initParm(addr);
        initSocket();
        addMultiGroup();
    }

private:
    MultiCast(MultiCast&) = delete;

    void initSocket()
    {
        fd_  = socket(AF_INET,SOCK_DGRAM,0);
        if(fd_ < 0)
        {
            std::cerr<<"init udp socket error"<<std::endl;
            return;
        }

        int block = 1;
        if(ioctl(fd_,FIONBIO,&block) < 0)
        {
            std::cerr<<"failed to FIONBIO"<<std::endl;
            return;
        }

        int reuse = 1;
        if(setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0)
        {
            std::cerr<<"fail to SO_REUSEADDR"<<std::endl;
            return;
        }

        int loop = 1;
        if(setsockopt(fd_,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop)) < 0)
        {
            std::cerr<<"fail to IP_MULTICAST_LOOP"<<std::endl;
            return;
        }

        int ttl = 255;
        if(setsockopt(fd_,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(ttl)) < 0)
        {
            std::cerr<<"fail to IP_MULTICAST_TTL"<<std::endl;
            return;
        }
    }


    virtual void addMultiGroup() = 0;


    void initParm(std::string addr)
    {
        auto p1 = split(addr,",");
        assert(p1.size() == 2);
        auto p2 = split(p1[0],":");
        assert(p2.size() == 2);

        multicastAddr_ = p2[0];
        port_ = atoi(p2[1].c_str());
        localIp_ = p1[1];

        PARM_STR(multicastAddr_);
        PARM_STR(port_);
        PARM_STR(localIp_);
    }
public:
    std::string multicastAddr_;
    int port_;
    std::string localIp_;

    int fd_;
    sockaddr_in perrAddr_;
};


class PubMultiCast : public MultiCast
{
public:
    PubMultiCast(std::string addr)
    {
        init(addr);
    }

    int32_t SendMsg(char *buf,int len)
    {
        if(fd_ < 0) return -1;
        int32_t ret = sendto(fd_,buf,len,0,
            (struct sockaddr *)&perrAddr_,sizeof(perrAddr_));
        if(ret < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return 0;
            }
            std::cerr<<"failed sendto, errno:"<<errno<<std::endl;
        }
        return ret;
    }
private:
    virtual void addMultiGroup()
    {
        perrAddr_.sin_family = AF_INET;
        perrAddr_.sin_addr.s_addr = inet_addr(multicastAddr_.c_str());
        perrAddr_.sin_port = htons(port_);

        struct in_addr localInterface;
        localInterface.s_addr = inet_addr(localIp_.c_str());

        if(setsockopt(fd_,IPPROTO_IP,IP_MULTICAST_IF,(char *)&localInterface,sizeof(localInterface)) < 0)
        {
            std::cerr<<"fail to IP_MULTICAST_IF"<<std::endl;
            return;
        }
    }

};


class SubMultiCast : public MultiCast
{
public:
    SubMultiCast(std::string addr)
    {
        init(addr);
    }

    int32_t RecvMsg(char *buf,int len)
    {
        if(fd_ < 0) return -1;
        int32_t ret = recvfrom(fd_,buf,len,0,
            (struct sockaddr *)&perrAddr_,&addrLen_);
        if(ret < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return 0;
            }
            std::cerr<<"failed sendto, errno:"<<errno<<std::endl;
        }
        return ret;
    }
private:
    virtual void addMultiGroup()
    {
        sockaddr_in localAddr;
        bzero(&localAddr,sizeof(localAddr));

        localAddr.sin_family = AF_INET;
        localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        localAddr.sin_port = port_;

        if(bind(fd_,(struct sockaddr *)&localAddr,sizeof(localAddr)) < 0)
        {
            std::cerr<<"failed to bind socket"<<std::endl;
            return;
        }
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(multicastAddr_.c_str());
        mreq.imr_interface.s_addr = inet_addr(localIp_.c_str());

        if(setsockopt(fd_,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)
        {
            std::cerr<<"fail to IP_ADD_MEMBERSHIP"<<std::endl;
            return;
        }
    }

private:
    socklen_t addrLen_ = sizeof(perrAddr_);
};