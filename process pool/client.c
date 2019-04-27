#include <func.h>

int recv_cycle(int newfd, void *p, int len){
    int ret, total = 0;
    char *p_start = (char*)p;
    while(total < len){
        ret = recv(newfd, p_start+total, len-total, 0);
        total += ret;
    }
    return 0;
}

int main(int argc, char* argv[]){
    ARGS_CHECK(argc, 3);

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &serv_addr.sin_addr);
    Connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("Connect success!\n");

    int data_len;
    char buf[1000] = {0};
    recv_cycle(sockfd, &data_len, 4);    //读出文件长度
    recv_cycle(sockfd, buf, data_len);   //读出文件名
    int fd = open(buf, O_CREAT|O_WRONLY, 0666);
    ERROR_CHECK(fd, -1, "open");

    while(1)
    {
        recv_cycle(sockfd, &data_len, 4);
        if(data_len > 0)
        {
            recv_cycle(sockfd, buf, data_len);
            write(fd, buf, data_len);

        }else{
            break;
        }
    }

    close(fd);
    close(sockfd);
    return 0;
}
