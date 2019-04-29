#include "pool.h"

int exit_fds[2];
void signal_func(int sig_num){
    Write(exit_fds[1], "q", 1);
}

int main(int argc, char* argv[]){
	ARGS_CHECK(argc, 4);

    //注册信号
    pipe(exit_fds);
    signal(SIGUSR1, signal_func);

	//初始化进程池
	int child_num = atoi(argv[3]);
	process_t *p_childs = (process_t*)calloc(child_num, sizeof(process_t));
	child_init(p_childs, child_num);

	//注册epoll
	int listenfd, epfd;
	listenfd = Tcp_init(argv[1], argv[2]); //监听套接字
	epfd = Epoll_create(1);
	struct epoll_event event, *evs;
	evs = (struct epoll_event*)calloc(child_num+2, sizeof(struct epoll_event));
	event.events = EPOLLIN;
	event.data.fd = listenfd;
	Epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
	event.data.fd = exit_fds[0];
    Epoll_ctl(epfd, EPOLL_CTL_ADD, exit_fds[0], &event);
	
    int i, j;
	for(i = 0; i < child_num; i++){
		event.data.fd = p_childs[i].fd;
		Epoll_ctl(epfd, EPOLL_CTL_ADD, p_childs[i].fd, &event);
	}

	int nfds, newfd;
	char ch;
	while(1){
		nfds = epoll_wait(epfd, evs, child_num+1, -1);
		for(i = 0; i < nfds; i++){
			if(evs[i].events == EPOLLIN && evs[i].data.fd == listenfd){
				newfd = Accept(listenfd, NULL, NULL);
				for(j = 0; j < child_num; j++){ //轮询空闲子进程去处理
					if(p_childs[j].busy == 0){
						printf("child %d is busy!\n", p_childs[j].pid);
						send_fd(p_childs[j].fd, newfd, 1);
						p_childs[j].busy = 1;
						break;
					}
				}
				close(newfd);
			}

            if(evs[i].data.fd == exit_fds[0]){ //收到SIGUSR1信号,退出
                printf("start exit!\n");
                close(listenfd);
                for(j = 0; j < child_num; j++){
                    send_fd(p_childs[j].fd, 0, 0);
                }
                for(j = 0; j < child_num; j++){
                    wait(NULL);
                }
                return 0;
            }

			for(j = 0; j < child_num; j++){
				if(evs[i].data.fd == p_childs[j].fd){
					//子进程处理完毕,返回
					read(p_childs[j].fd, &ch, 1);
					p_childs[j].busy = 0;
					printf("child %d isn't busy!\n", p_childs[j].pid);
				}
			}
		}
	}


	return 0;
}
