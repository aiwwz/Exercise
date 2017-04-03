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


void doit(int connectFd){
    /*创建并初始化读缓冲区*/
    rio_t rio;
    InitReadBuf(&rio);
    
    /*创建并读取一行请求行*/
    requestLine rl;
    readRequestLine(&rio, &rl);
    
    if(strcmp(&(rl.method), "GET")){
        clientError(connectFd, rl.)
    }
    
    /*创建并读取若干请求报头*/
    requestHdr *rhp;
    rhp = readRequestHdr(&rio);
    
    
}


void readRequestLine(rio_t *rp, requestLine *rlp){
    RioReadLine(rp, rlp, MAXLINE);
    
    scanf("%s %s %s", prl->method, prl->uri, prl->version);
}

void readRequestHdr(rio_t *rp){
    
    
}

void clientError(int connectFd, char *cause, char *errNo, char *shortMsg, char *longMsg){
    /*响应行和响应报头*/
    char *buf[MAXBUF];
    /*响应主体*/
    char *body[MAXBODY];
    
    /*填充响应主体*/
    fprintf(body, "<html><title>Mini-WebServer Error</title>");
    fprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    fprintf(body, "%s%s: %s\r\n", body, errNo, shortMsg);
    fprintf(body, "%s<p>%s: %s\r\n",body, longMsg, cause);
    fprintf(body, "%s<hr><em>The Mini-WebServer</em>\r\n", body);
    
    /*填充响应行和响应报头*/
    fprintf(buf, "HTTP/1.0 %s %s\r\n", errNo, shortMsg);
    fprintf(buf, "%sContent-type: text/html\r\n", buf);
    fprintf(buf, "%sContent-length: %d\r\n\r\n", strlen(body));
    
    write(connectFd, buf, strlen(buf));
    write(connectFd, body, strlen(body));
    
}

















