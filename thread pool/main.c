#include "factory.h"

int main(int argc, char *argv[]){
    ARGS_CHECK(argc, 5);

    factory_t fac;
    int thread_num = atoi(argv[3]); //线程数
    int max_elems = atoi(argv[4]);  //队列最大容量

    init_factory(&fac, thread_num, max_elems);
    start_factory(&fac);

    int listenfd, newfd;
    listenfd = Tcp_init(argv[1], argv[2]); 

    p_queue task_queue = fac.task_queue;
    while(1){
        newfd = Accept(listenfd, NULL, NULL);
        elem_t new_elem;
        new_elem.newfd = newfd;
        pthread_mutex_lock(&task_queue->mutex);
        enqueue(task_queue, &new_elem);
        pthread_mutex_unlock(&task_queue->mutex);
        pthread_cond_signal(&fac.cond);
    }

    return 0;
}
