#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <climits>
#include <atomic>

using namespace std;

////////////////////  Diagram for Thread Execution Order /////////////////

#if 0
                ----  A, B ----|
               |               |
-- > Sender ---|               |----- D, E ----- F
       ^       |               |                 |
       |        ----  C --------                 |
       |                                         |
       |                                         |
       ------------------------------------------

#endif

///////////////////////////////////////////////////////////////////////////

#define  EOK  0

pthread_mutex_t  groupLock;
pthread_cond_t   condvarGroup;

atomic<unsigned int> G1G2Counter {3}; 
atomic<unsigned int> G3Counter   {2}; 

int   PredicateSender  = 100;
int   PredicateVarG1   = 200;
int   PredicateVarG1A  = 201;
int   PredicateVarG1B  = 202;
int   PredicateVarG2   = 300;
int   PredicateVarG2C  = 301;
int   PredicateVarG3   = 400;
int   PredicateVarG3D  = 401;
int   PredicateVarG3E  = 402;
int   PredicateVarG4   = 500;

int   ExternalConstraintSender  = INT_MAX;
int   ExternalConstraintG1   = INT_MAX;
int   ExternalConstraintG1A  = INT_MAX;
int   ExternalConstraintG1B  = INT_MAX;
int   ExternalConstraintG2   = INT_MAX;
int   ExternalConstraintG2C  = INT_MAX;
int   ExternalConstraintG3   = INT_MAX;
int   ExternalConstraintG3D  = INT_MAX;
int   ExternalConstraintG3E  = INT_MAX;
int   ExternalConstraintG4   = INT_MAX;
int   PrevReciever           = INT_MAX;

void  *sender(void *);
void  *recieverG1A(void *);
void  *recieverG1B(void *);
void  *recieverG2C(void *);
void  *recieverG3D(void *);
void  *recieverG3E(void *);
void  *recieverG4F(void *);

FILE  * pFile = fopen ("debugfile.txt","w+");

int main(){
    int status;
    pthread_t  thread_sender;
    pthread_t  thread_recieverG1A,  thread_recieverG1B,  thread_recieverG2C;
    pthread_t  thread_recieverG3D,  thread_recieverG3E,  thread_recieverG4F;

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&groupLock, NULL);
    pthread_cond_init (&condvarGroup, NULL);

    pthread_create(&thread_sender,      NULL, sender,      (void *)0);
    pthread_create(&thread_recieverG1A, NULL, recieverG1A, (void *)0);
    pthread_create(&thread_recieverG1B, NULL, recieverG1B, (void *)0);
    pthread_create(&thread_recieverG2C, NULL, recieverG2C, (void *)0);
    pthread_create(&thread_recieverG3D, NULL, recieverG3D, (void *)0);
    pthread_create(&thread_recieverG3E, NULL, recieverG3E, (void *)0);
    pthread_create(&thread_recieverG4F, NULL, recieverG4F, (void *)0);

    pthread_join(thread_sender,      (void **)&status);
    pthread_join(thread_recieverG1A, (void **)&status);
    pthread_join(thread_recieverG1B, (void **)&status);
    pthread_join(thread_recieverG2C, (void **)&status);
    pthread_join(thread_recieverG3D, (void **)&status);
    pthread_join(thread_recieverG3E, (void **)&status);
    pthread_join(thread_recieverG4F, (void **)&status);

    return 0;
}


/* thread A */
void *sender(void *i){
    int retval;
    ExternalConstraintSender = PredicateSender;
    PrevReciever = PredicateVarG4;

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

              if (PrevReciever == PredicateVarG4){
                  ExternalConstraintG1  = PredicateVarG1;
                  ExternalConstraintG1A = PredicateVarG1A;
                  ExternalConstraintG1B = PredicateVarG1B;

                  ExternalConstraintG2  = PredicateVarG2;
                  ExternalConstraintG2C = PredicateVarG2C;

                  ExternalConstraintSender = INT_MAX;

                  fprintf(pFile, "Sender START >>>>  Send Signal to G1 and G2 \n") ;
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
           while ( !(ExternalConstraintG1 == PredicateVarG1) || ( !(ExternalConstraintG1A == PredicateVarG1A))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G1  Thread Id = %lu  :    G1A \n", pthread_self()) ;
          
           ExternalConstraintG1A = INT_MAX;
           --G1G2Counter;

           if (G1G2Counter == 0) {
               G1G2Counter  = 3;

               ExternalConstraintG1 = INT_MAX;
               ExternalConstraintG2 = INT_MAX;

               ExternalConstraintG3  = PredicateVarG3;
               ExternalConstraintG3D = PredicateVarG3D;
               ExternalConstraintG3E = PredicateVarG3E;

               fprintf(pFile, "Sender G1A >>>>  Send Signal to G3 \n") ;
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
           while ( !(ExternalConstraintG1 == PredicateVarG1) || ( !(ExternalConstraintG1B == PredicateVarG1B))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G1  Thread Id = %lu  :    G1B \n", pthread_self()) ;
          
           ExternalConstraintG1B = INT_MAX;
           --G1G2Counter;

           if (G1G2Counter == 0) {
               G1G2Counter  = 3;

               ExternalConstraintG1 = INT_MAX;
               ExternalConstraintG2 = INT_MAX;

               ExternalConstraintG3  = PredicateVarG3;
               ExternalConstraintG3D = PredicateVarG3D;
               ExternalConstraintG3E = PredicateVarG3E;

               fprintf(pFile, "Sender G1B >>>>  Send Signal to G3 \n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG2C(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {

           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintG2 == PredicateVarG2) || ( !(ExternalConstraintG2C == PredicateVarG2C))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G2  Thread Id = %lu  :    G2C \n", pthread_self()) ;
          
           ExternalConstraintG2C = INT_MAX;
           --G1G2Counter;

           if (G1G2Counter == 0) {
               G1G2Counter  = 3;

               ExternalConstraintG1 = INT_MAX;
               ExternalConstraintG2 = INT_MAX;

               ExternalConstraintG3  = PredicateVarG3;
               ExternalConstraintG3D = PredicateVarG3D;
               ExternalConstraintG3E = PredicateVarG3E;

               fprintf(pFile, "Sender G2C >>>>  Send Signal to G2 \n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG3D(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintG3 == PredicateVarG3)  || ( !(ExternalConstraintG3D == PredicateVarG3D))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G3  Thread Id = %lu  :    G3D \n", pthread_self()) ;
          
           ExternalConstraintG3D = INT_MAX;
           --G3Counter;

           if (G3Counter == 0) {
               G3Counter = 2;
               ExternalConstraintG3 = INT_MAX;
               ExternalConstraintG4 = PredicateVarG4;

               fprintf(pFile, "Sender G3D >>>>  Send Signal to G4 \n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}



void *recieverG3E(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintG3 == PredicateVarG3) || ( !(ExternalConstraintG3E == PredicateVarG3E))) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G3  Thread Id = %lu  :    G3E \n", pthread_self()) ;
          
           ExternalConstraintG3E = INT_MAX;
           --G3Counter;

           if (G3Counter == 0) {
               G3Counter = 2;
               ExternalConstraintG3 = INT_MAX;
               ExternalConstraintG4 = PredicateVarG4;
               fprintf(pFile, "Sender G3E >>>>  Send Signal to G4 \n") ;
           }

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);
    }
}


void *recieverG4F(void *i){
    int retval = 0;

    while (1) {
       retval = pthread_mutex_lock(&groupLock);
       if (retval == EOK) {
           // Here we are waiting on a Loop on CONSTRAINTS which is controlled by another Thread
           while ( !(ExternalConstraintG4 == PredicateVarG4)) {
               pthread_cond_wait(&condvarGroup, &groupLock);
           }

           fprintf(pFile, "Reciever:  G4  Thread Id = %lu  :    G4F \n", pthread_self()) ;
          
           ExternalConstraintG4 = INT_MAX;
           PrevReciever             =  PredicateVarG4;
           ExternalConstraintSender =  PredicateSender;

           fprintf(pFile, "Sender G4F >>>>  Send Signal to Sender(START) \n") ;
           fprintf(pFile, "\n <<<<<<<<<<<<<<  Moving Flow Graph to Sender Back >>>>>>>>>>>>>>> \n") ;

           pthread_cond_broadcast(&condvarGroup);
       }

       pthread_mutex_unlock(&groupLock);

    }
}

