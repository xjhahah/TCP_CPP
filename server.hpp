#pragma once

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <arpa/inet.h>
#include <strings.h>
#include <cstring>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unordered_map>

using namespace std;


class Sock
{
  private:
    int _sock;
    int _port;
    //string _ip;
  public:
    Sock(const int& port,int sock = -1)
      :_sock(sock)
      ,_port(port)
     // ,_ip(ip)
    {}
    void Socket()
    {
      //创建套接字
      _sock = socket(AF_INET,SOCK_STREAM,0);
      if(_sock < 0)
      {
        cerr<<"socket error!"<<endl;
        exit(2);
      }
    }
    void Bind()
    {
      //绑定端口号
      struct sockaddr_in local;
      bzero(&local,sizeof(local));
      local.sin_family = AF_INET;
      local.sin_port = htons(_port);
      //local.sin_addr.s_addr = inet_addr(_ip.c_str());
      local.sin_addr.s_addr = htonl(INADDR_ANY);  //不用每次都输入ip 
      if(bind(_sock,(struct sockaddr*)&local,sizeof(local))<0)
      {
        cerr<<"bind error"<<endl;
        exit(3);
      }
    }
    void Listen()
    {
      //监听客户端是否有链接
      if(listen(_sock,5)<0)
      {
        cerr<<"listen error"<<endl;
        exit(4);
      }
    }
    int Accept()
    {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);
      int sock = accept(_sock,(struct sockaddr*)&peer,&len);

      if(sock<0)
      {
        cerr<<"accept error"<<endl;
        return -1;
      }
      return sock;
    }
    ~Sock()
    {
      close(_sock);
    }
};

class Server 
{
  public:
  struct data_t 
  {
    Server* sp;
    int sock;
  };
  private:
    Sock sock;
    unordered_map<string,string> dict;
  public:
    Server(const int& port)
      :sock(port)
    {
      dict.insert(make_pair("hello","你好"));
      dict.insert(make_pair("bit","比特"));
      dict.insert(make_pair("XUST","西安科技大学"));
      dict.insert(make_pair("tiger","熊"));
    }

    void InitServer()
    {
      signal(SIGCHLD,SIG_IGN);
      sock.Socket();
      sock.Bind();
      sock.Listen();
    }
    void Service(int sock)
    {
      char buf[1024];
      while(1)
      {
        //读取客户端信息，并回传给客户端
        ssize_t s = read(sock,buf,sizeof(buf)-1);  //s == 0 代表对方关闭连接
        if(s>0)
        {
          buf[s] = 0;
          cout<<"client: "<< sock <<" echo# "<< buf <<endl;
          string key = buf;
          string val = dict[key];
          if(val.empty())
          {
            val = "null";
          }
          write(sock,val.c_str(),val.size());
        }
        else if(s == 0)
        {
          cout<<"---------client: "<<sock<<" quit---------"<<endl;
          break;
        }
        else{
          cerr<<"read error"<<endl;
          break;
        }
      }
      close(sock);  //必须关闭，否则会造成文件描述符泄漏
    }
    static void* ThreadRun(void* arg)
    {
      pthread_detach(pthread_self());
      data_t* p = (data_t*)arg;
      Server *sp = p->sp;
      int sock = p->sock;

      delete p;
      sp->Service(sock);
      //return (void*)0;
    }
    void Run()
    {
      while(1)
      {
        int new_sock = sock.Accept();
        if(new_sock<0)
        {
          cerr<<"accept error,reconnecting... "<<endl;
          continue;
        }
        cout<<"Get a new client..."<<endl;

        //多线程版本
        data_t *d = new data_t;
        d->sp = this;
        d->sock = new_sock;

        pthread_t tid;
        pthread_create(&tid,NULL,ThreadRun,(void*)d);


        //多进程版本
       // pid_t pid = fork();
       //if(pid<0)
       // {
       // cerr<<"fork error"<<endl;
       //   break;
       // }
       //else if(pid == 0)
       // {
       //   Service(new_sock);
       //   exit(0);
       // }
       // close(new_sock);  //父进程必须关闭 new_sock ,否则父进程可以用的文件描述符就会越来越少
      }
    }
    ~Server()
    {}
};











