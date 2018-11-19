typedef int buffer_item;
#define BUFFER_SIZE 5
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"
#include <unistd.h>

int insertPointer = 0, removePointer = 0;
/* The mutex lock */
pthread_mutex_t mutex;
/*semaphores*/
sem_t full;
sem_t empty; 
/*buffer*/
buffer_item buffer[BUFFER_SIZE];
/*buff counter*/
int counter;
pthread_attr_t attr; //Thread attributes
void *producer(void *param); /*producer thread*/
void *consumer(void *param); /*consumer thread*/
void *producer(void *param) {
   buffer_item item;
   while(TRUE) {
      /*random sleep time*/
      int rNum = rand() % BUFFER_SIZE;
      sleep(rNum);
      /*random number generator*/
      item = rand();
      if(insert_item(item)) {
         fprintf(stderr, " Producer report error condition\n");
      }
      else {
         printf("producer produced %d\n", item); 
      }
   }
}
/*Consumer Thread Process*/
void *consumer(void *param) {
   buffer_item item;
   while(TRUE) {
     // printf("Consuming");
      /* sleep for a random period of time */
      int rNum = rand() % BUFFER_SIZE;
      sleep(rNum);
      if(remove_item(&item)) {
         fprintf(stderr, "Consumer report error condition\n");
      }
      else if (item !=0){
         printf("consumer consumed %d\n", item);
      }
   }
}
int insert_item(buffer_item item)
{
    //Acquire Empty Semaphore
   sem_wait(&empty);
   //Acquire mutex lock to protect bufferpwd
   pthread_mutex_lock(&mutex);
   //Insert the item and move forward the pointer
   buffer[insertPointer]=item;
   insertPointer= (insertPointer + 1) % BUFFER_SIZE;
   //Release mutex lock and full semaphore
   pthread_mutex_unlock(&mutex);
   sem_post(&full);
   return 0; //success
}
int remove_item(buffer_item *item)
{
   //get full semaphore
   sem_wait(&full);
   //get mutex lock 
   pthread_mutex_lock(&mutex);
   //move pointer and remove item
   *item = buffer[insertPointer];
   buffer[insertPointer] = 0;
   insertPointer = (insertPointer -1) % BUFFER_SIZE;
   pthread_mutex_unlock(&mutex);
   sem_post(&empty);
   return 0;
}
int main(int argc, char *argv[]) {
   /* Loop counter */
   int i;
   /*Check for 3 user enetered arguments*/
   if(argc != 4) {
      fprintf(stderr, "Enter 3 integer aguments\n");
   }
   int mainSleepTime = atoi(argv[1]); /*Time in seconds for sleeping*/
   int producerNum = atoi(argv[2]); /*Number of producer threads*/
   int consumerNum = atoi(argv[3]); /*Number of consumer threads*/
   int fail;
   pthread_t pThread[producerNum];
   pthread_t cThread[consumerNum];
   /*setup*/
   pthread_mutex_init(&mutex, NULL);
   sem_init(&empty, 0, BUFFER_SIZE);
   sem_init(&full, 0, 0);
   /*loop for creating producer threads*/
   for(i = 0; i < producerNum; i++) {
      /*creating threads*/
	fail = pthread_create(&pThread[i], &attr, producer, NULL);
	  if (fail){
          printf("ERROR; return code from pthread_create() is %d\n", fail);
       }
    }
   /*loop for creating consumer threads*/
   for(i = 0; i < consumerNum; i++) {
      /*creating threads*/
	 fail = pthread_create(&cThread[i], &attr, consumer, NULL);
	  if (fail){
          printf("ERROR; return code from cthread_create() is %d\n", fail);
       }
   }
   /*sleep for specified time*/
   sleep(mainSleepTime);
   /*exit the program message, and code*/
   printf("Exit the program\n");
   exit(0);
}
