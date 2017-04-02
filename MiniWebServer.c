#include "MiniWebServer.h"


int main(int argc, char *argv[]){
    int listenFd, connectFd, port, clientLen;
    struct sockaddr_in clientAddr;
    
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    
    listenFd = OpenListenFd(port);  //创建监听描述符
    
    while(true){
        clientLen = sizeof(sockaddr_in)
        connectFd = Accept(listenFd, (SA*)&clientAddr, &clientLen);
        doit(connectFd);
        Close(connectFd);
    }
    
    return 0;
}