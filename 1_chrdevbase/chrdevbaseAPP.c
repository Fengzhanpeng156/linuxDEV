#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/*./chrdevbaseAPP <filename> <1:2> 1:read, 2:write*/

int main(int argc, char const *argv[])
{
    int ret = 0;
    int fd  = 0; 
    const char *filename;
    char readbuf[10];
    char writebuf[10];
    static char usrdata[] = {"usrdata"};


    filename = argv[1];

    fd = open(filename, O_RDWR );
    if (fd < 0)
    {
        printf("open file &s filed\r\n",filename);
        return -1;
        /* code */
    } 
//read
if(argc != 3){
    printf("error usage!\r\n");
    return -1;
}
if(atoi(argv[2]) == 1){
    ret = read(fd, readbuf, 10);
    if(ret < 0){
        printf("read file %s failed \r\n",filename);
    }else{
        printf("APP read data:%s\r\n",readbuf);

    }
}
if (atoi(argv[2]) == 2)
{
    memcpy(writebuf, usrdata, sizeof(usrdata));
    ret = write(fd, writebuf, 10);
    if(ret < 0){
        printf("write file %s failed \r\n",filename);
    }else{

    }
    /* code */
}

    ret = close(fd);
    if(ret < 0){
        printf("close file %s failed \r\n",filename);
    }

    return 0;
}
