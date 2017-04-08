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
        clientLen = sizeof(clientAddr);
        printf("ready!\n");
        connectFd = Accept(listenFd, (SA*)&clientAddr, &clientLen);
        printf("go!\n");
        doit(connectFd);
        Close(connectFd);
    }
    
    return 0;
}


void doit(int connectFd){
    /*创建并初始化读缓冲区*/
    printf("1\n");
    rio_t rio;
    InitReadBuf(&rio, connectFd);
    
    /*创建并读取一行请求行*/
    printf("2\n");
    requestLine rl;
    printf("2-1\n");
    readRequestLine(&rio, &rl);

    printf("%s\n", rl.method);    
    if(strcmp(rl.method, "GET")){
        clientError(connectFd, rl.method, "501", "Not Implemented", 
        "Mini-WebServer does not implement this method!");
        return;
    }
    
    /*创建并读取若干请求报头--使用链表维护*/
    printf("3\n");
    requestHdr *rhp; 
    rhp = readRequestHdr(&rio);
    
    /*解析URI*/
    printf("4\n");
    int isStatic;  //用于区分动静态服务
    char *filename, *cgiArgs;
    struct stat sbuf;
    isStatic = parseUri(rl.uri, filename, cgiArgs);
    
    printf("4-1 %s %s %d\n", filename, cgiArgs, isStatic);
    if(stat(filename, &sbuf) < 0){ //没有找到文件
        clientError(connectFd, rl.method, "404", "Not Found",
        "Mini-WebServer couldn't found this file");
        return;
    }
    
    if(isStatic){  //服务静态内容
        printf("5\n");
        if((S_IRUSR & sbuf.st_mode) && S_ISREG(sbuf.st_mode)){ //可访问且是普通文件
            printf("5-1\n");
            serveStatic(connectFd, filename, sbuf.st_size);
        }
        else{
            printf("5-2\n");
            clientError(connectFd, rl.method, "403", "Forbidden", 
            "Mini-WebServer couldn't read this file");
        }
    }
    else{  //服务动态内容
        printf("6\n");
        if((S_IXUSR & sbuf.st_mode) && S_ISREG(sbuf.st_mode)){ //可执行且是普通文件
            printf("6-1\n");
            serveDynamic(connectFd, filename, cgiArgs);
        }
        else{
            printf("6-2\n");
            clientError(connectFd, rl.method, "403", "Forbidden",
            "Mini-WebServer couldn't run this file");
        }
    }
}


void readRequestLine(rio_t *rp, requestLine *rlp){
    char buf[MAXLINE];
    printf("2-2\n");
    RioReadLine(rp, buf, MAXLINE);
    printf("2-3\n");
    sscanf(buf, "%s %s %s", rlp->method, rlp->uri, rlp->version);
    printf("2-4\n");
}


requestHdr *readRequestHdr(rio_t *rp){
    requestHdr *rhp, *rhq;
    rhp = rhq = NULL;
    char buf[MAXHDR];
    
    /*创建第一个请求报头结点(如果有的话)*/
    RioReadLine(rp, buf, MAXHDR);
    if(!strcmp(buf, "\r\n")){
        requestHdr *tmp = (requestHdr*)malloc(sizeof(requestHdr));
        sscanf(buf, "%s %s", tmp->headerName, tmp->headerData);
        rhp = rhq = tmp;        
    }
    
    while(strcmp(buf, "\r\n")){
        requestHdr *tmp = (requestHdr*)malloc(sizeof(requestHdr));
        sscanf(buf, "%s %s", tmp->headerName, tmp->headerData);
        rhp->next = tmp;
        rhp = tmp;
    }
    return rhp;
}


void clientError(int connectFd, char *cause, char *errNo, char *shortMsg, char *longMsg){
    /*响应行和响应报头*/
    char buf[MAXBUF];
    /*响应主体*/
    char body[MAXBODY];
    
    /*填充响应主体*/
    sprintf(body, "<html><title>Mini-WebServer Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errNo, shortMsg);
    sprintf(body, "%s<p>%s: %s\r\n",body, longMsg, cause);
    sprintf(body, "%s<hr><em>The Mini-WebServer</em>\r\n", body);
    
    /*填充响应行和响应报头*/
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errNo, shortMsg);
    sprintf(buf, "%sContent-Type: text/html\r\n", buf);
    sprintf(buf, "%sContent-Length: %d\r\n\r\n", buf, (int)strlen(body));
    
    RioWriteN(connectFd, buf, strlen(buf));
    
}


int parseUri(char *uri, char *filename, char *cgiArgs){
    char *ptr;
    if(!strstr(uri, "cgi_bin")){  //静态内容 
        strcpy(cgiArgs, ""); //置空
        strcpy(filename, "."); //默认目录为当前目录
        strcat(filename, uri);
        /*文件名缺省*/
        if(uri[strlen(uri) - 1] = '/'){
            strcat(filename, "index.html");
            return 1;
        }
    }
    else{  //动态内容
        ptr = strstr(uri, "?");
        if(ptr){
            *ptr = '\0';
            strcpy(cgiArgs, ptr + 1);
        }
        else{
            strcpy(cgiArgs, "");
        }
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}


void serveStatic(int connectFd, char *filename, int filesize){
    /*发送响应行和响应报头*/
    char filetype[MAXLINE];
    char buf[MAXBUF];
    getFileType(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); //响应行
    sprintf(buf, "%sMIME-Version: 1.0\r\n", buf); //响应报头
    sprintf(buf, "%sServer: Mini-WebServer\r\n", buf);
    sprintf(buf, "%sContent-Type: %s\r\n", buf, filetype);
    sprintf(buf, "%sContent-Length: %d\r\n\r\n", buf, filesize);
    RioWriteN(connectFd, buf, strlen(buf));
    
    /*发送响应主体*/
    int bodyFd;
    char *bodyPtr;
    bodyFd = Open(filename, O_RDONLY, 0);
    bodyPtr = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, bodyFd, 0);
    RioWriteN(connectFd, bodyPtr, filesize);
    Close(bodyFd);
    Munmap(bodyPtr, filesize);
}


void serveDynamic(int connectFd, char *filename, char *cgiArgs){
    /*发送响应行和响应报头*/
    char buf[MAXBUF];
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); //响应行
    sprintf(buf, "%sServer: Mini-WebServer\r\n\r\n", buf); //响应报头
    RioWriteN(connectFd, buf, strlen(buf));
    
    /*运行cgi程序*/
    const char *emptyList[] = { NULL };
    if(Fork() == 0){
        setenv("QUERY_STRING", cgiArgs, 1);
        Dup2(connectFd, STDOUT_FILENO); //重定向标准输出
        Execve((const char*)filename, emptyList, (const char**)environ); //运行cgi程序 
    }
    Wait(NULL);
}


void getFileType(char *filename, char *filetype){
    if(strstr(filename, ".html")){
        strcpy(filetype, "text/html");
    }
    else if(strstr(filename, ".jepg")){
        strcpy(filetype, "image/jepg");
    }
    else if(strstr(filename, ".gif")){
        strcpy(filetype, "image/gif");
    }
    else{
        strcpy(filetype, "text/plain"); //无格式文本文件
    }
    
}
