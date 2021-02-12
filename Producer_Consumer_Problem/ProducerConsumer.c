
#include <pthread.h>   
#include <stdio.h>
#define MAX 50
#define BUFLEN 24
#define MAX_COUNT 15
#define NUMTHREAD 3      /* number of threads */

void * consumer(int *id);
void * producer(int *id);

char buffer[BUFLEN];
char source[BUFLEN];
int rCount = 0, wCount = 0;
int buflen;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t nonEmpty  = PTHREAD_COND_INITIALIZER;
pthread_cond_t full  = PTHREAD_COND_INITIALIZER;
int thread_id[NUMTHREAD]  = {0,1,2};
int i = 0, j = 0;

main()
{
  int i;
  /* define the type to be pthread */
  pthread_t thread[NUMTHREAD];

  strcpy(source,"hello world!");
  buflen = strlen(source);
  /* create 2 threads*/
  /*
    pthread_create(&thread[2], NULL, (void *)watch, &thread_id[2]);
  */
  /* create one consumer and one producer */
  pthread_create(&thread[0], NULL, (void *)consumer, &thread_id[0]);
  pthread_create(&thread[1], NULL, (void *)producer, &thread_id[1]);
  pthread_create(&thread[2], NULL, (void *)producer, &thread_id[2]);
  
  for(i=0; i< NUMTHREAD ; i++)
    {
      pthread_join(thread[i], NULL);
    }
}

void * consumer(int *id)
{
  /* lock the variable */
  pthread_mutex_lock(&count_mutex);

  while(j < MAX)
    {
      /* wait for the buffer to have something in it */
      pthread_cond_wait(&nonEmpty, &count_mutex);

      /* take the char from the buffer and increment the rCount */
      printf("          consumed value is :%c: by %d\n", buffer[rCount], *id);
      rCount = (rCount + 1) % BUFLEN;
      fflush(stdout);
      j ++;

      if (j < (MAX - 2))     
	/* Last sleep might leave the condition un-processed.
	 * So we prohibit sleep towards the end
	 */
	if (rand()%100 < 30)
	  sleep(rand()%3);

    }
  /* signal the producer that the buffer has been consumed */
  /* pthread_cond_signal(&full);*/
  /*unlock the variable*/
  pthread_mutex_unlock(&count_mutex);
}

void * producer(int *id)
{

  while (i < MAX)
    {
      /* lock the variable */
      pthread_mutex_lock(&count_mutex);
      /* wait for the buffer to have space */
      /* pthread_cond_wait(&full, &count_mutex);*/
      strcpy(buffer,"");
      buffer[wCount] = source[wCount%buflen];
      printf("%d produced :%c: by  :%d:\n",i, buffer[wCount], *id);
      fflush(stdout);
      wCount = (wCount + 1) % BUFLEN;
      i ++;
      /* for the condition notify the thread */
      pthread_cond_signal(&nonEmpty);
      /*unlock the variable*/
      pthread_mutex_unlock(&count_mutex);

      if (i < (MAX - 2))     
	/* Last sleep might leave the condition un-processed.
	 * So we prohibit sleep towards the end
	 */
	if (rand()%100 >= 30)
	  sleep(rand()%3);

    }
}
