#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include <unistd.h>
#include "../rocket/common/log.h"
#include "../rocket/common/config.h"
// #include "../rocket/net/tcp/tcp_client.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/net/tcp/tcp_server.h"

void test_connect() {

    // 调用 connect 连接 server
    // write 一个字符串
    // 等待read返回结果

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        ERRORLOG("invalid fd %d", fd);
        exit(0);
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12346);
    inet_aton("127.0.0.1", &server_addr.sin_addr);

    int rt = connect(fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

    // DEBUGLOG("connect success");

    std::string msg = "hello rocket!";
    
    rt = write(fd, msg.c_str(), msg.length());

    DEBUGLOG("success write %d bytes, [%s]", rt, msg.c_str());

    char buf[100];
    rt = read(fd, buf, 100);
    DEBUGLOG("success read %d bytes, [%s]", rt, std::string(buf).c_str());


}

int main() {

  rocket::Config::SetGlobalConfig("../conf/rocket.xml");
  rocket::Logger::InitGlobalLogger();

//   rocket::IPNetAddr addr("127.0.0.1", 12345);
//   DEBUGLOG("create addr %s", addr.toString().c_str());

//   rocket::Config::SetGlobalConfig(NULL);

//   rocket::Logger::InitGlobalLogger(0);

  test_connect();
  return 0;
  
}