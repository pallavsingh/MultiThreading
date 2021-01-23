#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
using namespace std;

#define NUMTHREADS 4

pthread_key_t    glob_var_key_1;
pthread_key_t    glob_var_key_2;

void do_something(){
    cout << "Entering  Function" << __func__ << "Line Number :" << __LINE__ << endl;    

    int * glob_spec_var_1 = (int*) pthread_getspecific(glob_var_key_1);
    printf("Thread %d before mod value is %d\n", (unsigned int) pthread_self(), *glob_spec_var_1);
    *glob_spec_var_1 += 1;
    printf("Thread %d after mod value is %d\n", (unsigned int) pthread_self(), *glob_spec_var_1);

    int * glob_spec_var_2 = (int*) pthread_getspecific(glob_var_key_2);
    printf("Thread %d before mod value is %d\n", (unsigned int) pthread_self(), *glob_spec_var_2);
    *glob_spec_var_2 += 1;
    printf("Thread %d after mod value is %d\n", (unsigned int) pthread_self(), *glob_spec_var_2);

    cout << "Exsiting Function" << __func__ << "Line Number :" << __LINE__ << endl;    
}

void* thread_func(void *arg)
{
    int *p1 = (int *)malloc(sizeof(int));
    *p1 = 1;

    int *p2 = (int *)malloc(sizeof(int));
    *p2 = 100;
   
    // We maintain 1:1 Mapping of key w.r.t shared critical section.
    // Here we Create Key for data being shared across threads
    pthread_setspecific(glob_var_key_1, p1);
    pthread_setspecific(glob_var_key_2, p2);

    // Here same data will be accesed by threads twice and modidifed.
    // We will generate key w.r.t (PID + TID + NodeNumber) to uniqe Indentification 
    // Here key is associated till the Life of calling Threads
    // Here kernel must Know the from which Thread Activation record for Function is getting Called.
    // and it will Fetch data for that Thread for the data strcuture.

    do_something();
    do_something();

    // Here we are setting Specific Key to NULL.
    pthread_setspecific(glob_var_key_1, NULL);
    pthread_setspecific(glob_var_key_2, NULL);

    free(p1);
    free(p2);

    pthread_exit(NULL);
}

int main(void)
{
    pthread_t threads[NUMTHREADS];
    int i;

    pthread_key_create(&glob_var_key_1, NULL);
    pthread_key_create(&glob_var_key_2, NULL);

    for (i=0; i < NUMTHREADS; i++)
        pthread_create(&threads[i],NULL,thread_func,NULL);

    for (i=0; i < NUMTHREADS; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
