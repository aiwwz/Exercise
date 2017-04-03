#ifndef MiniWebServer_H
#define MiniWebServer_H
#include "public.h"
#include <file.h>

#define MAXBUF 8192
#define MAXBODY 8192
#define MAXLINE 4096
#define MAXHDR 1024

/*请求行结构*/
typedef struct{
    char method[MAXLINE];
    char uri[MAXLINE];
    char version[MAXLINE];
}requestLine;

/*请求报头结构--使用单链表存储*/
typedef struct{
    char headerName[MAXHDR];
    char headerData[MAXHDR];
    requestHdr *next;
}requestHdr;


/************************************************
* 函数功能：处理客户端连接请求。
* 输入参数：conneceFd -- 已连接描述符
* 输出参数：无
*   返回值：无
*   创建人：王志军
* 创建时间：2017.4.1
* 修改备注：无
************************************************/
void doit(int connectFd);


/************************************************
* 函数功能：读取请求行,进行错误处理。
* 输入参数：*rp -- 与已连接描述符绑定的缓冲区
* 输出参数：*prl -- 请求行结构
*   返回值：无
*   创建人：王志军
* 创建时间：2017.4.1
* 修改备注：无
************************************************/
void readRequestLine(rio_t *rp, requestLine *rlp);


/************************************************
* 函数功能：读取请求报头，进行错误处理。
* 输入参数：*rp -- 与已连接描述符绑定的缓冲区
* 输出参数：无
*   返回值：*rhp -- 有报头则返回报头链表首部指针
            NULL -- 无请求报头
              -1 -- 失败
*   创建人：王志军
* 创建时间：2017.4.1
* 修改备注：无
************************************************/
requestHdr *readRequestHdr(rio_t *rp);
                                   

/************************************************
* 函数功能：向客户端发送错误提示页面。
* 输入参数：conneceFd -- 已连接描述符
               *cause -- 错误原因
               *errNo -- 错误状态代码
            *shortMsg -- 状态消息
             *longMsg -- 描述
* 输出参数：无
*   返回值：无
*   创建人：王志军
* 创建时间：2017.4.1
* 修改备注：无
************************************************/
void clientError(int connectFd, char *cause, char *errNo, char *shortMsg, char *longMsg);
 
#endif MiniWebServer_H