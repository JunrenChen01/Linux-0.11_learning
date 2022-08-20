/*consumer.c*/
#define   __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/sem.h>

_syscall2(sem_t*,sem_open,const char *,name,unsigned int,value);
_syscall1(int,sem_wait,sem_t*,sem);
_syscall1(int,sem_post,sem_t*,sem);
_syscall1(int,sem_unlink,const char *,name);
_syscall1(void*,shmat,int,shmid);
_syscall2(int,shmget,int,key,int,size);


#define NUMBER 20
#define BUFSIZE 10

sem_t  *empty, *full, *mutex;
int main()
{
    int  i,shmid,data;
    int *p;
    int  buf_out = 0;
    if((mutex = sem_open("mutex",1)) == NULL)
    {
        perror("sem_open() error!\n");
        return -1;
    }
    if((empty = sem_open("empty",10)) == NULL)
    {
        perror("sem_open() error!\n");
        return -1;
    }
    if((full = sem_open("full",0)) == NULL)
    {
        perror("sem_open() error!\n");
        return -1;
    }
	
	printf("consumer start.\n");
    fflush(stdout);
    /*shmid = shmget("buffer");*/
	shmid = shmget(1234, BUFSIZE*sizeof(int));
	if(shmid == -1)
    {
        return -1;
    }
    p = (int *)shmat(shmid);

    for( i = 0; i < NUMBER; i++ )
    {
        sem_wait(full);
        sem_wait(mutex);
        data = p[buf_out];
        buf_out = (buf_out + 1) % BUFSIZE;
	printf("%d:  %d\n",getpid(),data);
        fflush(stdout);
        sem_post(mutex);
        sem_post(empty);
    }
    printf("consumer end.\n");
    fflush(stdout);
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("mutex");

    return 0;
}

