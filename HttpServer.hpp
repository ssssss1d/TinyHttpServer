#pragma once
#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "ThreadPool.hpp"

const static int PORT = 8081; // 默认端口

    /*线程池版本*/
class HttpServer
{
private:
  int _port;  // 通过上层设置端口
  bool _stop; // 停止服务，默认值false，表示不停止

public:
  HttpServer(int port = PORT)
      : _port(port), _stop(false)
  {}

  void Loop()
  {
    signal(SIGPIPE, SIG_IGN); // 服务端写客户端关闭，此时会导致服务端进程接收到SIGPIPE新型号，直接被os杀掉，忽略这个信号，防止因这种情况导致服务端关闭

    TcpServer::GetInstance(_port)->InitServer(); // 前三步，socket、bind、listen
    LOG(INFO, "server start to access links");

    // 接收连接并执行任务
    while (!_stop)
    {
      sockaddr_in peer;               // 对端信息
      memset(&peer, 0, sizeof(peer)); // 初始化0
      socklen_t len = sizeof(peer);

      // 接受连接
      int sock = accept(TcpServer::GetInstance(_port)->ListenSocket(), (sockaddr *)&peer, &len);
      if (sock < 0)
      { // 接收连接失败，继续接收下一个
        LOG(ERROR, "get new link failed");
        continue;
      }
      // 获取客户端IP和port
      std::string clientIP = inet_ntoa(peer.sin_addr);
      int clientPort = ntohs(peer.sin_port);

      // 打印一下客户端信息
      LOG(INFO, std::move("get new link, sock[" + std::to_string(sock) + "], client message::" + clientIP + ":" + std::to_string(clientPort)));

      // 接收到链接后将任务放置到任务队列中，让线程池中的某个线程去执行任务
      Task* t = new Task(sock);
      ThreadPool::GetInstance()->PushTask(t);
    }
  }
};



    /*新来链接就创建一个线程的版本*/
// class HttpServer
// {
// private:
//   int _port;  // 通过上层设置端口
//   bool _stop; // 停止服务，默认值false，表示不停止

// public:
//   HttpServer(int port = PORT)
//       : _port(port), _stop(false)
//   {}

//   void Loop()
//   {
//     signal(SIGPIPE, SIG_IGN); // 服务端写客户端关闭，此时会导致服务端进程接收到SIGPIPE新型号，直接被os杀掉，忽略这个信号，防止因这种情况导致服务端关闭

//     TcpServer *tsvr = TcpServer::GetInstance(_port);
//     tsvr->InitServer(); // 前三步，socket、bind、listen
//     LOG(INFO, "server start to access links");

//     // 接收连接并执行任务
//     while (!_stop)
//     {
//       sockaddr_in peer;               // 对端信息
//       memset(&peer, 0, sizeof(peer)); // 初始化0
//       socklen_t len = sizeof(peer);

//       // 接受连接
//       int sock = accept(tsvr->ListenSocket(), (sockaddr *)&peer, &len);
//       if (sock < 0)
//       { // 接收连接失败，继续接收下一个
//         LOG(ERROR, "get new link failed");
//         continue;
//       }
//       // 获取客户端IP和port
//       std::string clientIP = inet_ntoa(peer.sin_addr);
//       int clientPort = ntohs(peer.sin_port);

//       // 打印一下客户端信息
//       LOG(INFO, std::move("get new link, client message::" + clientIP + ":" + std::to_string(clientPort)));

//       // 接收到新连接后，新起一个线程，让新线程去处理新连接的任务
//       int *p = new int(sock);
//       pthread_t tid;
//       pthread_create(&tid, nullptr, ThreadCallBack::Handler, (void *)p);

//       pthread_detach(tid); // 线程分离，os自动回收新线程，不需要让主线程join等待
//     }
//   }
// };
