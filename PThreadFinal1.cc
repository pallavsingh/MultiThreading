#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <climits>

using namespace std;

#define  EOK  0

pthread_mutex_t  Lock1;
pthread_cond_t   condvarLock1;

int counter = 0;

int PredicateSender = 50;
int PredicateVar1 = 100;
int PredicateVar2 = 200;
int PredicateVar3 = 300;

int ResetVar1 = 10;
int ResetVar2 = 20;
int ResetVar3 = 30;

int ExternalConstraint;

void *sender(void *);
void *reciever1(void *);
void *reciever2(void *);
void *reciever3(void *);

int * ptr1 = (int *)malloc(sizeof(int));
int * ptr2 = (int *)malloc(sizeof(int));
int * ptr3 = (int *)malloc(sizeof(int));

FILE * pFile = fopen ("debugfile.txt","w+");

int main(){
    int status;
    pthread_t thread_sender, thread_reciever1, thread_reciever2, thread_reciever3;
    *ptr1 = 0;
    *ptr2 = 0;
    *ptr3 = 0;

    pthread_attr_t attr;
    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&Lock1, NULL);
    pthread_cond_init (&condvarLock1, NULL);

    pthread_create(&thread_sender,    NULL, sender,    (void *)0);
    pthread_create(&thread_reciever1, NULL, reciever1, (void *)0);
    pthread_create(&thread_reciever2, NULL, reciever2, (void *)0);
    pthread_create(&thread_reciever3, NULL, reciever3, (void *)0);

    pthread_join(thread_sender,    (void **)&status);
    pthread_join(thread_reciever1, (void **)&status);
    pthread_join(thread_reciever2, (void **)&status);
    pthread_join(thread_reciever3, (void **)&status);
}

/* thread A */
void *sender(void *i){
    int retval;
    ExternalConstraint = PredicateSender;

    // Here we are creating Virtual Looping Running Block. This Block is tightly coupled 
    // with this thread.We Need Another Global variable to Avoid blocking of Conditional 
    // wait with a breaking condition inside it and Conditional Signal API.
    // Since the shceduling of Threads cannot be controlled and is determined by OS, we 
    // need to contraint them by external global variable.
    
    while  (1) {
          retval = pthread_mutex_lock(&Lock1);
          if (retval == EOK) {
              while ( !(ExternalConstraint == PredicateSender)) {
                  pthread_cond_wait(&condvarLock1, &Lock1);
              }

              ExternalConstraint = PredicateVar1;
              fprintf(pFile, "Sender : Send Signal to reciever1 \n") ;
              pthread_cond_broadcast(&condvarLock1);
          }

          pthread_mutex_unlock(&Lock1);
    }

}

void *reciever1(void *i){
    int retval = 0;
    while (1) {
       retval = pthread_mutex_lock(&Lock1);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraint == PredicateVar1)) {
               pthread_cond_wait(&condvarLock1, &Lock1);
           }
           fprintf(pFile, "Reciever1 Thread Id = %lu -------- Y1  :: counter  :  %d\n", pthread_self(), ++counter) ;
           ExternalConstraint = PredicateVar2;
           pthread_cond_broadcast(&condvarLock1);
       }
       pthread_mutex_unlock(&Lock1);
    }

}

void *reciever2(void *i){
    int retval = 0;
    while (1) {
       retval = pthread_mutex_lock(&Lock1);
       if (retval == EOK) {

           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraint == PredicateVar2)) {
               pthread_cond_wait(&condvarLock1, &Lock1);
           }

           fprintf(pFile, "Reciever2 Thread Id = %lu -------- Y2  :: counter  :  %d\n", pthread_self(), ++counter) ;
           ExternalConstraint = PredicateVar3;
           pthread_cond_broadcast(&condvarLock1);
       }
       pthread_mutex_unlock(&Lock1);
    }

}

void *reciever3(void *i){
    int retval = 0;
    while (1) {
       retval = pthread_mutex_lock(&Lock1);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraint == PredicateVar3)) {
               pthread_cond_wait(&condvarLock1, &Lock1);
           }

           fprintf(pFile, "Reciever3 Thread Id = %lu -------- Y3  :: counter  :  %d\n", pthread_self(), ++counter) ;
           ExternalConstraint = PredicateSender;
           pthread_cond_broadcast(&condvarLock1);
       }
       pthread_mutex_unlock(&Lock1);
    }

}

