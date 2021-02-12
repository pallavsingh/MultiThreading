#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 1. How do we create Thread Cancellation points in Code.
 2. Under what scenrio, we should Enable and Disable the Thread Cancellation.
 3. Whats the diffrence between Defferred Cancallation and Syncronous Cancellation. 
*/

void cleanupHandler(void *parm) {
  printf("Inside cancelation cleanup handler\n");
}

void *threadfunc(void *parm) {
  unsigned int  i=0;
  int rc=0, oldState=0;

  printf("Entered secondary thread\n");
  pthread_cleanup_push(cleanupHandler, NULL);

  rc = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldState);
  rc = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldState);
  // Here new and pending request will actted immediatly. Here user donot require
  // to set cancellation Point.
 
  // rc = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldState);
  // Its is by default. Here cancellcation is held pending till cancellation point.  
  // Here User require to set cancellation point explicitly. 

  printf("pthread_setcancelstate()\n", rc);
  sleep(2);
  while (1) {
       printf("Secondary thread is now looping\n");
       ++i;
       sleep(1);

      /* pthread_testcancel() function shall create a cancellation point  in 
       * calling thread.It has no effect if cancelability is disabled. */
       // pthread_testcancel();

       if (i == 5) {
          printf("Cancel state set to ENABLE\n");
          rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldState);
          printf("pthread_setcancelstate(2)\n", rc);
       }
  } 
  
  pthread_cleanup_pop(0);
  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t             thread;
  int                   rc=0;
  void                 *status=NULL;

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Create thread using the NULL attributes\n");
  rc = pthread_create(&thread, NULL, threadfunc, NULL);
  printf("pthread_create(NULL)\n", rc);

  sleep(1);
  printf("Cancel the thread\n");
  rc = pthread_cancel(thread);
  printf("pthread_cancel()\n", rc);

  rc = pthread_join(thread, &status);
  if (status != PTHREAD_CANCELED) {
    printf("Thread returned unexpected result!\n");
    exit(1);
  }

  printf("Main completed\n");
  return 0;
}

