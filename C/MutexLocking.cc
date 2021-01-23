#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMTHRDS 4
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;

void * critical_section(void * arg){
   int * ptr1 = (int *)malloc(sizeof(int));
   int * ptr2 = (int *)malloc(sizeof(int));
   
   // Here we Aquire Lock and Update our Critical Section.
   pthread_mutex_lock (&mutexsum);
   *ptr1 = 0;
   *ptr1 = *ptr1 + 1;

   *ptr2 = 0;
   *ptr2 = *ptr2 + 1;
   pthread_mutex_unlock (&mutexsum);
   // Here we Release Lock and Update our Critical Section.

   printf("Thread id %lu | Value of ptr1 = %d  ptr2 = %d \n", pthread_self(), *ptr1, *ptr2);

   free(ptr1);
   free(ptr2);
}

int main(){
  int i;
  void * status;
  pthread_attr_t attr;
  pthread_mutex_init(&mutexsum, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for(i = 0; i < NUMTHRDS; i++)
     pthread_create(&callThd[i], &attr, critical_section, (void *)&i);

  pthread_attr_destroy(&attr);

  for(i = 0; i < NUMTHRDS; i++)
     pthread_join(callThd[i], &status);

  pthread_mutex_destroy(&mutexsum);
  pthread_exit(NULL);

}
