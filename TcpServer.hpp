#pragma once

#include "Common.h"

const static int WAIT_LENGTH = 5; // 全连接队列长度为  WAIT_LENGTH + 1

class TcpServer
{
private:
  int _port;       // 端口
  int _listenSock; // 监听套接字

  // 懒汉模式
private:
  static TcpServer *_tsvr; // 单例模式

  TcpServer(int port)
      : _port(port), _listenSock(-1)
  {}

  TcpServer(const TcpServer &cp) = delete;
  TcpServer &operator=(const TcpServer &cp) = delete;

public:
  static TcpServer *GetInstance(int port)
  {
    // 双if ，效率高且安全
    if (_tsvr == nullptr)
    {
      static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // static的锁相当于全局的锁，可以直接用PTHREAD_MUTEX_INITIALIZER来初始化
      pthread_mutex_lock(&mtx);
      if (_tsvr == nullptr)
      {
        _tsvr = new TcpServer(port);
      }
      pthread_mutex_unlock(&mtx);
    }

    return _tsvr;
  }

  void InitServer()
  {
    Socket(); // 创建监听套接字
    Bind();   // 绑定IP与Port
    Listen(); // 设置监听状态
  }

  // 创建套接字
  void Socket()
  {
    //      网络通信    字节流    给0默认协议，此时为TCP
    _listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenSock < 0)
    { // 创建监听套接字失败
      LOG(FATAL, "create listen sock fail");
      exit(1);
    }

    int opt = 1;
    // 设置地址复用，防止服务器崩掉后进入TIME_WAIT，短时间连不上当前端口
    setsockopt(_listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    LOG(INFO, "create listen sock success");
  }

  // bind绑定
  void Bind()
  {
    sockaddr_in local;
    memset(&local, 0, sizeof(local));   // 给sin_zero置0
    local.sin_family = AF_INET;         // 协议家族和创建socket时相同
    local.sin_port = htons(_port);      // 主机转网络字节序
    local.sin_addr.s_addr = INADDR_ANY; // 将服务端IP直接给为0.0.0.0，能接受到任意网卡传来的信息
    if (bind(_listenSock, (sockaddr *)&local, sizeof(local)) < 0)
    { // 绑定失败
      LOG(INFO, "bind fail");
      exit(2);
    }
    LOG(INFO, "bind success");
  }

  // listen设置监听状态
  void Listen()
  {
    if (listen(_listenSock, WAIT_LENGTH) < 0)
    { // 监听失败
      LOG(INFO, "listen fail");
      exit(3);
    }
    LOG(INFO, "listen success");
  }

  int ListenSocket()
  {
    return _listenSock;
  }

  ~TcpServer()
  {
    if(_listenSock > 0)
    {
      close(_listenSock);
    }
  }
};

TcpServer *TcpServer::_tsvr = nullptr; // 单例模式对象
