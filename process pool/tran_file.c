#include "pool.h"

void tran_file(int newfd){
	train_t train;
	
    //发送filename
    train.data_len = strlen(FILENAME);
	strcpy(train.buf, FILENAME);
    int ret; 
    ret = send(newfd, &train, train.data_len+4, 0);
    if(ret == -1) return;

    int fd = Open(FILENAME, O_RDONLY);
   
    //发送filesize
    struct stat st;
    fstat(fd, &st);
    printf("file size is %ld\n", st.st_size);
    ret = send(newfd, &st.st_size, sizeof(off_t), 0);
    if(ret == -1) return;

    //发送文件   
    while((train.data_len = Read(fd, train.buf, sizeof(train.buf)))){
        ret = send(newfd, &train, 4+train.data_len, 0);
        if(ret == -1) return;
    }

    //发送结束标志
    send(newfd, &train, 4, 0); //此时train.date_len为0
}
