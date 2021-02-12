#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define  EOK   0
pthread_mutex_t   recursive_lock_a;
void   *a(void *);
int     recursive_func(int counter);
int    *ptr1 = (int *)malloc(sizeof(int));

int main(){
    int status;
    pthread_t thread_a;
    pthread_mutexattr_t   lock_attr;
    *ptr1 = 0;

    pthread_mutexattr_init(&lock_attr);
    pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_lock_a, &lock_attr);

    pthread_create(&thread_a, NULL, a, (void *)0);
    pthread_join(thread_a, (void **)&status);
}

void *a(void *i){
    recursive_func(4);
    printf("Value inside *ptr1 = %d \n", *ptr1);
}

int recursive_func( int counter){
    if(counter == 0){
       struct timespec ts;
       ts.tv_sec = 5;
       ts.tv_nsec = 0;
       pthread_mutex_timedlock(&recursive_lock_a, &ts);
       printf("Sleeping for a short time holding the recurive mutex\n");
       usleep(300);
       return 0;
    }

    counter--;
    pthread_mutex_lock(&recursive_lock_a);
    printf("Success : recursive Lock = %d \n", counter );
    *ptr1 = *ptr1 + 1;
    recursive_func(counter);
    pthread_mutex_unlock(&recursive_lock_a);
    printf("Success : recursive Un-Lock = %d \n", counter );
}
