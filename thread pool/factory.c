#include "factory.h"

void* download_file(void *p){
    while(1);

    return NULL;
}

void init_factory(p_factory pfac, int thread_num, int max_elems){
    pfac->p_threads = (pthread_t*)calloc(thread_num, sizeof(pthread_t));
    if(pfac->p_threads == NULL){
        FatalError("Out of space!");
    }
    pfac->thread_num = thread_num;
    pthread_cond_init(&pfac->cond, NULL);
    init_queue(pfac->task_queues, max_elems);
    pfac->start_flag = 0;
}

void start_factory(p_factory pfac){
    int i;
    if(pfac->start_flag == 0){
        for(i = 0; i < pfac->thread_num; i++){
            pthread_create(pfac->p_threads+i, NULL, download_file, NULL);
            printf("创建线程%d成功!\n", i);
        } 
    }
}
