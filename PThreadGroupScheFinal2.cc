#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <climits>
#include <atomic>

using namespace std;

////////////////////////////  Thread Execution Control Order /////////////////////////

#if 0
                           |---- A
                           |
                |---- G1 --|---- B
                |          |
                |          |---- C
                |
-----> Sender ---
                |
                |          |---- X
                |          |
                |---- G2 --|---- Y
                           |
                           |---- Z

Generate Level Order Printing (Order of Threads Execution on Same Level donot depend)

> Sender
>>  G1, G2
>>>  A, B, C, D, E, F

#endif

//////////////////////////////////////////////////////////////////////////////////

#define  EOK  0

pthread_mutex_t  groupLock;
pthread_cond_t   condvarGroup;

atomic<unsigned int> L1Counter {2}; 
atomic<unsigned int> L2Counter {6}; 

int   Sink  = 10;
int   PredicateSender    = 100;
int   PredicateVarG1     = 200;
int   PredicateVarG1A    = 201;
int   PredicateVarG1B    = 202;
int   PredicateVarG1C    = 203;
int   PredicateVarG2     = 300;
int   PredicateVarG2D    = 301;
int   PredicateVarG2E    = 302;
int   PredicateVarG2F    = 303;
int   PredicateVarL1   = 400;
int   PredicateVarL2   = 401;

int   ExternalConstraintSender  = INT_MAX;
int   ExternalConstraintG1      = INT_MAX;
int   ExternalConstraintG1A     = INT_MAX;
int   ExternalConstraintG1B     = INT_MAX;
int   ExternalConstraintG1C     = INT_MAX;
int   ExternalConstraintG2      = INT_MAX;
int   ExternalConstraintG2D     = INT_MAX;
int   ExternalConstraintG2E     = INT_MAX;
int   ExternalConstraintG2F     = INT_MAX;
int   PrevReciever              = INT_MAX;
int   ExternalConstraintL    = INT_MAX;

void  *sender(void *);
void  *recieverG1(void *);
void  *recieverG2(void *);
void  *recieverG1A(void *);
void  *recieverG1B(void *);
void  *recieverG1C(void *);
void  *recieverG2D(void *);
void  *recieverG2E(void *);
void  *recieverG2F(void *);

FILE  * pFile = fopen ("debugfile.txt","w+");

int main(){
    int status;
    pthread_t  thread_sender;
    pthread_t  thread_recieverG1,   thread_recieverG2;
    pthread_t  thread_recieverG1A,  thread_recieverG1B,  thread_recieverG1C;
    pthread_t  thread_recieverG2D,  thread_recieverG2E,  thread_recieverG2F;

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&groupLock, NULL);
    pthread_cond_init (&condvarGroup, NULL);

    pthread_create(&thread_sender,      NULL,  sender,      (void *)0);
    pthread_create(&thread_recieverG1,  NULL,  recieverG1,  (void *)0);
    pthread_create(&thread_recieverG2,  NULL,  recieverG2,  (void *)0);
    pthread_create(&thread_recieverG1A, NULL,  recieverG1A, (void *)0);
    pthread_create(&thread_recieverG1B, NULL,  recieverG1B, (void *)0);
    pthread_create(&thread_recieverG1C, NULL,  recieverG1C, (void *)0);
    pthread_create(&thread_recieverG2D, NULL,  recieverG2D, (void *)0);
    pthread_create(&thread_recieverG2E, NULL,  recieverG2E, (void *)0);
    pthread_create(&thread_recieverG2F, NULL,  recieverG2F, (void *)0);

    pthread_join(thread_sender,      (void **)&status);
    pthread_join(thread_recieverG1,  (void **)&status);
    pthread_join(thread_recieverG2,  (void **)&status);
    pthread_join(thread_recieverG1A, (void **)&status);
    pthread_join(thread_recieverG1B, (void **)&status);
    pthread_join(thread_recieverG1C, (void **)&status);
    pthread_join(thread_recieverG2D, (void **)&status);
    pthread_join(thread_recieverG2E, (void **)&status);
    pthread_join(thread_recieverG2F, (void **)&status);

    return 0;
}


/* thread A */
void *sender(void *i){
    int retval;
    ExternalConstraintSender = PredicateSender;
    PrevReciever = Sink;

    // Here we are creating Virtual Looping Running Block. This Block is tightly coupled 
    // with this thread.We Need Another Global variable to Avoid blocking of Conditional 
    // wait with a breaking condition inside it and Conditional Signal API.
    // Since the shceduling of Threads cannot be controlled and is determined by OS, we 
    // need to contraint them by external global variable.
    
    while  (1) {
          retval = pthread_mutex_lock(&groupLock);
          if (retval == EOK) {
              while ( !(ExternalConstraintSender == PredicateSender)) {
                  pthread_cond_wait(&condvarGroup, &groupLock);
              }

              if (PrevReciever == Sink){
                  ExternalConstraintG1  = PredicateVarG1;
                  ExternalConstraintG2  = PredicateVarG2;
                  ExternalConstraintL   = PredicateVarL1;
                  L1Counter = 2;

                  fprintf(pFile, "Sender START >>>>  Send Signal to G1 and G2 \n") ;
              }

              ExternalConstraintSender = INT_MAX;

              pthread_cond_broadcast(&condvarGroup);
          }

          pthread_mutex_unlock(&groupLock);
    }
}

void *recieverG1(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL1) || ( !(ExternalConstraintG1 == PredicateVarG1))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G1  Thread Id = %lu  :    G1 \n", pthread_self()) ;
          
           ExternalConstraintG1 = INT_MAX;
           --L1Counter;

           if (L1Counter == 0) {
               L2Counter = 6;

               ExternalConstraintL  = PredicateVarL2;

               ExternalConstraintG1A = PredicateVarG1A;
               ExternalConstraintG1B = PredicateVarG1B;
               ExternalConstraintG1C = PredicateVarG1C;

               ExternalConstraintG2D = PredicateVarG2D;
               ExternalConstraintG2E = PredicateVarG2E;
               ExternalConstraintG2F = PredicateVarG2F;

               fprintf(pFile, "Sender G1 >>>>  Send Signal to G1A G1B G1C G2D G2E G2F \n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG2(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL1) || ( !(ExternalConstraintG2 == PredicateVarG2))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G2  Thread Id = %lu  :    G2 \n", pthread_self()) ;
          
           ExternalConstraintG2 = INT_MAX;
           --L1Counter;

           if (L1Counter == 0) {
               L2Counter = 6;

               ExternalConstraintL  = PredicateVarL2;

               ExternalConstraintG1A = PredicateVarG1A;
               ExternalConstraintG1B = PredicateVarG1B;
               ExternalConstraintG1C = PredicateVarG1C;

               ExternalConstraintG2D = PredicateVarG2D;
               ExternalConstraintG2E = PredicateVarG2E;
               ExternalConstraintG2F = PredicateVarG2F;

               fprintf(pFile, "Sender G2 >>>>  Send Signal to G1A G1B G1C G2D G2E G2F \n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG1A(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL2) || ( !(ExternalConstraintG1A == PredicateVarG1A))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G1  Thread Id = %lu  :    G1A \n", pthread_self()) ;
          
           ExternalConstraintG1A = INT_MAX;
           --L2Counter;

           if (L2Counter == 0) {
                   ExternalConstraintL  = INT_MAX;
                   PrevReciever  =  Sink;
                   ExternalConstraintSender =  PredicateSender;
                   fprintf(pFile, "Sender G1A >>>>>>>>>>>>>>>>>>  Send Signal to START >>>>>>>>>>>>>>>>>\n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG1B(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL2) || ( !(ExternalConstraintG1B == PredicateVarG1B))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G1  Thread Id = %lu  :    G1B \n", pthread_self()) ;
          
           ExternalConstraintG1B = INT_MAX;
           --L2Counter;

           if (L2Counter == 0) {
                   ExternalConstraintL  = INT_MAX;
                   PrevReciever  =  Sink;
                   ExternalConstraintSender =  PredicateSender;
                   fprintf(pFile, "Sender G1B >>>>>>>>>>>>>>>>>>  Send Signal to START >>>>>>>>>>>>>>>>>\n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG1C(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL2) || ( !(ExternalConstraintG1C == PredicateVarG1C))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G1  Thread Id = %lu  :    G1C \n", pthread_self()) ;
          
           ExternalConstraintG1C = INT_MAX;
           --L2Counter;

           if (L2Counter == 0) {
                   ExternalConstraintL  = INT_MAX;
                   PrevReciever  =  Sink;
                   ExternalConstraintSender =  PredicateSender;
                   fprintf(pFile, "Sender G1C >>>>>>>>>>>>>>>>>>  Send Signal to START >>>>>>>>>>>>>>>>>\n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}



void *recieverG2D(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL2) || ( !(ExternalConstraintG2D == PredicateVarG2D))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G2  Thread Id = %lu  :    G2D \n", pthread_self()) ;
          
           ExternalConstraintG2D = INT_MAX;
           --L2Counter;

           if (L2Counter == 0) {
                   ExternalConstraintL  = INT_MAX;
                   PrevReciever  =  Sink;
                   ExternalConstraintSender =  PredicateSender;
                   fprintf(pFile, "Sender G2D >>>>>>>>>>>>>>>>>>  Send Signal to START >>>>>>>>>>>>>>>>>\n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG2E(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL2) || ( !(ExternalConstraintG2E == PredicateVarG2E))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G2  Thread Id = %lu  :    G2E \n", pthread_self()) ;
          
           ExternalConstraintG2E = INT_MAX;
           --L2Counter;

           if (L2Counter == 0) {
                   ExternalConstraintL  = INT_MAX;
                   PrevReciever  =  Sink;
                   ExternalConstraintSender =  PredicateSender;
                   fprintf(pFile, "Sender G2E >>>>>>>>>>>>>>>>>>  Send Signal to START >>>>>>>>>>>>>>>>>\n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG2F(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintL == PredicateVarL2) || ( !(ExternalConstraintG2F == PredicateVarG2F))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G2  Thread Id = %lu  :    G2F \n", pthread_self()) ;
          
           ExternalConstraintG2F = INT_MAX;
           --L2Counter;

           if (L2Counter == 0) {
                   ExternalConstraintL  = INT_MAX;
                   PrevReciever  =  Sink;
                   ExternalConstraintSender =  PredicateSender;
                   fprintf(pFile, "Sender G2F >>>>>>>>>>>>>>>>>>  Send Signal to START >>>>>>>>>>>>>>>>>\n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}
