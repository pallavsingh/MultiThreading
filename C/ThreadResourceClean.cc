#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/*
cleanup_handlers for threads pushed in stack are only called when
1. thread call pthread_exit();
2. thread get killed from pthread_cancel().
3. These Handlers are never invoked, when thread make gracefull return from 
   first calling function associated with pthread_create().

pthread_cleanup_push() most likley is implemented as a macro introducing an
open brace { which expects a (corrsponding) pthread_cleanup_pop()in the same
context. The latter then servers the closing brace }
*/

/////////////////////////////////////////////////////////////////////////////// 

void cleanup_handler0(void *arg){
  printf("cleanup_handler_0\n");
  return;
}
 
void cleanup_handler1(void *arg){
  printf("cleanup_handler_1\n");
  return;
}

void cleanup_handler2(void *arg){
  printf("cleanup_handler_2\n");
  return;
}

void cleanup_handler3(void *arg){
  printf("cleanup_handler_3\n");
  return;
}

///////////////////////////////////////////////////////////////////////////////

static void func1(){
  pthread_cleanup_push(cleanup_handler1, NULL);
  sleep(3);
  // pthread_exit(NULL);  // Enabling it will cause Handler poped from stack
  pthread_cleanup_pop(0);
}

static void func2(){
  pthread_cleanup_push(cleanup_handler2, NULL);
  func1();
  pthread_cleanup_pop(0);
}

static void func3(){
  pthread_cleanup_push(cleanup_handler3, NULL);
  func2();
  pthread_cleanup_pop(0);
}

void* start_new_thread(void *arg){
  printf("start new thread ...\n");
  pthread_cleanup_push(cleanup_handler0, NULL);
  func3();
 
  printf("end new thread...\n");
  pthread_cleanup_pop(0);
  return NULL;
}

/////////////////////////////////////////////////////////////////////////

int main(){
  pthread_t thread;
  int ret;
  printf("start main thread....\n");
 
  if( pthread_create(&thread, NULL, start_new_thread, NULL ) != 0 )
    return 1;
  sleep(1);

  /* cancel!! it causes cleanup handler associated with context of 
     each function call getting invoked
   */
  
  pthread_cancel(thread);
  pthread_join(thread, (void**)&ret);
  printf("end main thread....\n");
  return 0;
}
