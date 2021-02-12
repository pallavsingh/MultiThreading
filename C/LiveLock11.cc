#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int run_thread_a = 0;
pthread_mutex_t run_lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t run_cond_a = PTHREAD_COND_INITIALIZER;

int run_thread_b = 0;
pthread_mutex_t run_lock_b = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t run_cond_b = PTHREAD_COND_INITIALIZER;

void *a(void *);
void *b(void *);

int main(){
    int status;
    pthread_t thread_a;
    pthread_t thread_b;

    pthread_create(&thread_a, NULL, a, (void *)0);
    pthread_create(&thread_b, NULL, b, (void *)0);

    pthread_join(thread_a, (void **)&status);
    pthread_join(thread_b, (void **)&status);
}

/* thread A */
void *a(void *i){
    while (1) {
       /* Wait for Thread A to be runnable */
       pthread_mutex_lock(&run_lock_a);
       while (!run_thread_a)
           pthread_cond_wait(&run_cond_a, &run_lock_a);
       run_thread_a = 0;
       pthread_mutex_unlock(&run_lock_a);

       /* Do some work */

       /* Now wake thread B */
       pthread_mutex_lock(&run_lock_b);
       run_thread_b = 1;
       pthread_cond_signal(&run_cond_b);
       pthread_mutex_unlock(&run_lock_b);
    }
}

/* thread B */
void *b(void *i){
    while (1) {
       /* Wait for Thread B to be runnable */
       pthread_mutex_lock(&run_lock_b);
       while (!run_thread_b)
           pthread_cond_wait(&run_cond_b, &run_lock_b);
       run_thread_b = 0;
       pthread_mutex_unlock(&run_lock_b);

       /* Do some work */

       /* Now wake thread A */
       pthread_mutex_lock(&run_lock_a);
       run_thread_a = 1;
       pthread_cond_signal(&run_cond_a);
       pthread_mutex_unlock(&run_lock_a);
    }
}
