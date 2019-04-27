#include "pool.h"

void send_fd(int sfd,int fd){
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct iovec iov[2];
	char buf1[10];
	char buf2[10];
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	Sendmsg(sfd,&msg,0);
}

void recv_fd(int sfd,int *fd){
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct iovec iov[2];
	char buf1[10];
	char buf2[10];
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	Recvmsg(sfd,&msg,0);
	*fd=*(int*)CMSG_DATA(cmsg);
}
