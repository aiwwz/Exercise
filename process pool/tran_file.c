#include "pool.h"

void tran_file(int newfd){
	train_t train;
	
    //发送filename
    train.data_len = strlen(FILENAME);
	strcpy(train.buf, FILENAME);
    send(newfd, &train, train.data_len+4, 0);

    //发送文件   
    int fd = Open(FILENAME, O_RDONLY);
    while((train.data_len = Read(fd, train.buf, sizeof(train.buf)))){
        Send(newfd, &train, 4+train.data_len, 0);
    }

    //发送结束标志
    send(newfd, &train, 4, 0); //此时train.date_len为0
}
