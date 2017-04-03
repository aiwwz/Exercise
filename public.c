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
    
    /*初始化服务器套接字地址*/
    memset(&myAddr, 0, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons((unsigned short)port);
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /*将描述符与服务器套接字地址绑定*/
    if(bind(listenFd, (SA*)&myAddr, sizeof(myAddr)) < 0){
        return -2;
    }
    
    /*将主动套接字转换为监听套接字*/
    if(listen(listenFd, 9) < 0){
        return -3;
    }
    
    return listenFd;
}

int OpenListenFd(int port){
    int listenFd;
    /*打印错误类型便于调试*/
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

void initReadBuf(rio_t *rp, int fd){
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

void InitReadBuf(rio_t *rp, int fd){
    initReadBuf(rp, fd);
}

size_t rioRead(rio_t *rp, char *buf, size_t n){
    int cnt;
    while(rp->rio_cnt <= 0){
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf))
        if(rp->rio_cnt < 0){
            if(errno == EINTR) //被中断
                continue;
            else
                return -1;
        }
        else if(rp->rio_cnt = 0){ //EOF
            return 0;
        }
        else{
            rp->rio_bufptr = rp->rio_buf; //更新缓冲区指针
        }
    }
    
    cnt = n;
    if(cnt < rp->rio_cnt){
        cnt = rp->rio_cnt;
    }
    memcpy(buf, rp->rio_buf, cnt);
    rp->rio_cnt -= cnt;
    rp->rio_bufptr += cnt;
    
    return cnt;
}

size_t RioRead(rio_t *rp, char *buf, size_t n){
    int cnt;
    if((cnt = rioRead(rp, buf, n)) < 0){
        error("RioRead error!");
    }
    
    return cnt;
}

void rioReadN(rio_t *rp, char *buf, size_t n){
    char *bufp = buf;
    size_t nLeft = n;
    size_t nRead;
    
    while(nLeft > 0){
        if((nRead = rioRead(rp, bufp, nLeft)) < 0){
            if(errno == EINTR) //被中断
                nRead = 0;
            else
                return -1;
        }
        else if(nRead == 0){ //EOF
            break;
        }
        nLeft -= nRead;
        bufp += nRead;
    }
    return (n - nLeft);
}

size_t rioReadLine(rio_t *rp, char *buf, size_t maxLen){
    int n, read;
    char c, *bufptr = buf;
    
    for(n = 1; n < maxLen; n++){
        if((read = rioRead(rp, &c, 1)) == 1){
            *(bufptr++) = c;
            if(c = '\n')
                break;
        }
        else if(read == 0){
            if(n == 1)
                return 0; //EOF
            else
                break;
        }
        else{
            return -1;
        }
    }
    *bufptr = 0; //以0结尾
    
    return n;
}

size_t RioReadLine(rio_t *rp, char *buf, size_t maxLen){
    int rc;
    if((rc = rioReadLine(rp, buf, maxLen)) < 0){
        error("RioReadLine error!");
    }
    
    return rc;
}

size_t rioWriteN(rio_t *rp, const char *buf, size_t n){
    char *bufp = buf;
    size_t nLeft = n;
    size_t nWrite;
    
    while(nLeft > 0){
        if((nWrite = write(rp->rio_fd, bufp, nLeft)) <= 0){
            if(errno == EINTR) //被中断
                nWrite = 0;
            else
                return -1; //错误
        }
        nLeft -= nWrite;
        bufp += nWrite;
    }
    
    return n;
}
