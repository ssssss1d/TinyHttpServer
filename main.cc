#include"HttpServer.hpp"

#include<memory>

// 用法
void Usage(std::string s)
{
  cout << "Usage:\n\t" << s << " port" << endl;
}

int main(int argc, char* argv[])
{
  if(argc != 2)
  {
    Usage(argv[0]);
    exit(4);
  }

  std::shared_ptr<HttpServer> hsvr(new HttpServer(atoi(argv[1])));
  hsvr->Loop();
   

  return 0;
}
