#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>

int getNumberOfCpus( void ){
   long nprocs       = -1;
   long nprocs_max   = -1;

   # ifdef _SC_NPROCESSORS_ONLN
      nprocs = sysconf( _SC_NPROCESSORS_ONLN );
      nprocs_max = sysconf( _SC_NPROCESSORS_CONF );
      return nprocs; 
   #else
      return 0;
   #endif
}

void *pthread_Message( void *ptr ) {
    sleep(10);
    char *message;
    message = (char *) ptr;
    printf("%s \n", message);
    cpu_set_t  l_cpuSet;
    int  l_maxCpus;
    int  j;
    unsigned long  l_cpuBitMask;

    CPU_ZERO(&l_cpuSet);
    printf("get affinity %d\n",pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &l_cpuSet));
    printf ("thread id %u\n", pthread_self());      

    if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &l_cpuSet) == 0 )
        for (int i = 0; i < 4; i++)
            if (CPU_ISSET(i, &l_cpuSet))
                printf("XXXCPU: CPU %d\n", i);

    for (long i=0; i< 10000000000; ++i);
}

int main()
{
    pthread_t thread1, thread2, thread3, thread4;
    pthread_t threadArray[4];
    cpu_set_t cpu1, cpu2, cpu3, cpu4;
    const char *thread1Msg = "Thread 1";
    const char *thread2Msg = "Thread 2";
    const char *thread3Msg = "Thread 3";
    const char *thread4Msg = "Thread 4";
    int thread1Create, thread2Create, thread3Create, thread4Create, i, temp;

    thread1Create = pthread_create(&thread1, NULL, &pthread_Message, (void*)thread1Msg);
    thread2Create = pthread_create(&thread2, NULL, &pthread_Message, (void*)thread2Msg);
    thread3Create = pthread_create(&thread3, NULL, &pthread_Message, (void*)thread3Msg);
    thread4Create = pthread_create(&thread4, NULL, &pthread_Message, (void*)thread4Msg);


    CPU_ZERO(&cpu1);
    CPU_SET(0, &cpu1);
    temp = pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpu1);
    printf("setaffinity = %d\n", temp);
    printf("Set returned by pthread_getaffinity_np() contained:\n");
    for (i = 0; i < CPU_SETSIZE; i++)
        if (CPU_ISSET(i, &cpu1))
            printf("CPU1: CPU %d\n", i);


    CPU_ZERO(&cpu2);
    CPU_SET(1, &cpu2);
    temp = pthread_setaffinity_np(thread2, sizeof(cpu_set_t), &cpu2);
    for (i = 0; i < CPU_SETSIZE; i++)
        if (CPU_ISSET(i, &cpu2))
            printf("CPU2: CPU %d\n", i);


    CPU_ZERO(&cpu3);
    CPU_SET(2, &cpu3);
    temp = pthread_setaffinity_np(thread3, sizeof(cpu_set_t), &cpu3);
    for (i = 0; i < CPU_SETSIZE; i++)
        if (CPU_ISSET(i, &cpu3))
            printf("CPU3: CPU %d\n", i);


    CPU_ZERO(&cpu4);
    CPU_SET(3, &cpu4);
    temp = pthread_setaffinity_np(thread4, sizeof(cpu_set_t), &cpu4);
    for (i = 0; i < CPU_SETSIZE; i++)
        if (CPU_ISSET(i, &cpu4))
            printf("CPU4: CPU %d\n", i);


    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    return 0;
}
