#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define EOK 0

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

void *a(void *);
void *b(void *);

int * ptr1 = (int *)malloc(sizeof(int));
int * ptr2 = (int *)malloc(sizeof(int));

int main(){
    int status;
    pthread_t thread_a;
    pthread_t thread_b;
    *ptr1 = 0;
    *ptr2 = 0;

    pthread_create(&thread_a, NULL, a, (void *)0);
    pthread_create(&thread_b, NULL, b, (void *)0);

    pthread_join(thread_a, (void **)&status);
    pthread_join(thread_b, (void **)&status);
}

// TryLock Thread donot get Blocked when they are not able to Aquire Lock,
// but with Lock Threads get Blocked

/* thread A */
void *a(void *i){
    int aquired_resource = 0;
    int retval;
    while (1) {
       retval = pthread_mutex_trylock(&lock_a);
       if (retval == EOK) {
           printf("Thread Id = %lu Aquire Lock A \n", pthread_self());
           usleep(200);
           retval = pthread_mutex_trylock(&lock_b);
           if (retval == EOK) {
              printf("Thread Id = %lu Aquire Lock B \n", pthread_self());
              aquired_resource = 1;
              *ptr1 = *ptr1 + 1;
              *ptr2 = *ptr2 + 1;
              pthread_mutex_unlock(&lock_b);
              printf("Thread Id = %lu Release Lock B \n", pthread_self());
           } 
       }
       pthread_mutex_unlock(&lock_a);
       printf("Thread Id = %lu Release Lock A \n", pthread_self());
       if (aquired_resource)
           break;
    }
}

/* thread B */
void *b(void *i){
    int aquired_resource = 0;
    int retval;
    while (1) {
       retval = pthread_mutex_trylock(&lock_b);
       if (retval == EOK) {
           printf("Thread Id = %lu Aquire Lock B \n", pthread_self());
           usleep(300);
           retval = pthread_mutex_trylock(&lock_a);
           if (retval == EOK) {
               printf("Thread Id = %lu Aquire Lock A \n", pthread_self());
              aquired_resource = 1;
              *ptr1 = *ptr1 + 1;
              *ptr2 = *ptr2 + 1;
              pthread_mutex_unlock(&lock_a);
              printf("Thread Id = %lu Release Lock A \n", pthread_self());
           } 
       }
       pthread_mutex_unlock(&lock_b);
       printf("Thread Id = %lu Release Lock B \n", pthread_self());
       if (aquired_resource)
          break;
    }
}
