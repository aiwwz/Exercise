#ifndef MiniWebServer_H
#define MiniWebServer_H
#include "public.h"
 
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


 
 
#endif MiniWebServer_H