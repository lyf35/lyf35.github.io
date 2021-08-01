#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char** argv)
{
    bool append=false;
    int filenum=argc-1;
    if(strcmp(argv[1],"-a")==0)
    {
        append=true;
        filenum--;
    }
    int fd_array[filenum];
    for(int i=0;i<filenum;i++)
    {
        int flags = O_RDWR | O_CREAT;
        if(append)
        {
            flags|=O_APPEND;
        }
        else
        {
            flags|=O_TRUNC;
        }
        int fd=open(argv[i+1+append],flags,S_IRUSR|S_IWUSR);
        if(fd==-1)
        {
            return -1;
        }
        fd_array[i]=fd;
    }
    
    while(true)
    {
        void* buffer=malloc(1024);
        ssize_t num_read;
        num_read=read(0,buffer,1024);
        if(num_read==0||num_read==-1)
        {
            break;
        }
        for(int i=0;i<filenum;i++)
        {
            if(write(fd_array[i],buffer,num_read)==-1)
            {
                return -1;
            }
        }
    }
    for(int i=0;i<filenum;i++)
    {
        if(close(fd_array[i])==-1)
        {
            return -1;
        }
    }
    return 0;
}