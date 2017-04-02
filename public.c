#include "public.h"

void error(char *msg){
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

int openListenFd(int port){
    int listenFd;
    struct sockaddr_in myAddr;
    
    if((listenFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }
    
    /*??????????????????*/
    memset(&myAddr, 0, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons((unsigned short)port);
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /*???????????????????????????*/
    if(bind(listenFd, (SA*)&myAddr, sizeof(myAddr)) < 0){
        return -2;
    }
    
    /*????????????????????????????????????????9*/
    if(listen(listenFd, 9) < 0){
        return -3;
    }
    
    return listenFd;
}

int OpenListenFd(int port){
    int listenFd;
    /*?????????????????*/
    if((listenFd = openListenFd(port)) < 0){
        switch(listenFd){
        case -1: 
            error("Socket error!");
            break;
        case -2:
            error("Bind error!");
            break;
        case -3:
            error("Listen error!");
            break;
        default:
            break;
        }
        exit(0);
    }
    return listenFd;
}

int Accept(int listenFd, struct sockaddr *addr, int *clientLen){
    int connectFd;
    if((connectFd = accept(listenFd, addr, clientLen)) < 0){
        error("Accept error!");
    }
    
    return connectFd;
}

void Close(int connectFd){
    if(close(connectFd) < 0){
        error("Close error!");
    }
}









