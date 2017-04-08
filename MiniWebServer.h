#ifndef MiniWebServer_H
#define MiniWebServer_H
#include "public.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUF 8192
#define MAXBODY 8192
#define MAXLINE 1024
#define MAXHDR 1024

extern const char **environ;

/*请求行结构*/
typedef struct{
    char method[MAXLINE];
    char uri[MAXLINE];
    char version[MAXLINE];
}requestLine;

/*请求报头结构--使用单链表存储*/
typedef struct rhNode requestHdr;
struct rhNode{
    char headerName[MAXHDR];
    char headerData[MAXHDR];
    requestHdr *next;
};


/************************************************
* 函数功能：处理客户端连接请求。
* 输入参数：connectFd -- 已连接描述符
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
*   创建人：王志军
* 创建时间：2017.4.1
* 修改备注：无
************************************************/
requestHdr *readRequestHdr(rio_t *rp);
                                   

/************************************************
* 函数功能：向客户端发送错误提示页面。
* 输入参数：connectFd -- 已连接描述符
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
 
 
/************************************************
* 函数功能：解析请求行中的URI。
* 输入参数：*uri -- URI信息
* 输出参数：*filename -- 请求的文件名
             *cgiArgs -- 动态内容的参数
*   返回值：1 -- 静态内容请求
            0 -- 动态内容请求
*   创建人：王志军
* 创建时间：2017.4.3
* 修改备注：无
************************************************/
int parseUri(char *uri, char *filename, char *cgiArgs);


/************************************************
* 函数功能：服务静态内容。
* 输入参数：connectFd -- 已连接描述符
            *filename -- 文件名(包含路径)
             filesize -- 文件大小
* 输出参数：无
*   返回值：无
*   创建人：王志军
* 创建时间：2017.4.3
* 修改备注：无
************************************************/
void serveStatic(int connectFd, char *filename, int filesize);

 
/************************************************
* 函数功能：服务动态内容。
* 输入参数：connectFd -- 已连接描述符
            *filename -- 文件名(包含路径)
             *cgiArgs -- cgi程序参数
* 输出参数：无
*   返回值：无
*   创建人：王志军
* 创建时间：2017.4.3
* 修改备注：无
************************************************/
void serveDynamic(int connectFd, char *filename, char *cgiArgs);


/************************************************
* 函数功能：通过文件名后缀获取文件类型。
* 输入参数：*filename -- 文件名
* 输出参数：*filetype -- 文件类型
*   返回值：无
*   创建人：王志军
* 创建时间：2017.4.3
* 修改备注：无
************************************************/
void getFileType(char *filename, char *filetype);

 
#endif /*MiniWebServer_H*/
