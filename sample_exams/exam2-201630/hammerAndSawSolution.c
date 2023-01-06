/* Copyright 2016 Rose-Hulman */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/**
   In this system, there are are two different process, that each have
   2 steps.

   There are two resources, hammer and saw.  Each of these are
   exclusive (i.e. only one thread may use them at once).

   Process A step 1 requires the saw.
   Process A step 2 requires the saw and hammer.

   Process B step 1 requires the hammer.
   Process B step 2 requires the hammer and saw.

   We want the processes to take advantage of the maxiumum possible
   amount of paralellism. That is, both A1 and B1 should be able to
   execute at the same time.

   Write a solution that enforces that the resources are exclusive and
   does not have deadlocks.
   
**/

sem_t hammer;
sem_t saw;

void *processA(void *arg) {

  sem_wait(&saw);
  printf("Starting A1 (requires saw) \n");
  sleep(1);
  printf("Finishing A1 \n");
  sem_post(&saw);
  
  sem_wait(&hammer);
  sem_wait(&saw);
  printf("Starting A2 (requires saw and hammer) \n");
  sleep(1);
  printf("Finishing A2 \n");

  sem_post(&saw);
  sem_post(&hammer);
  return NULL;
}

void *processB(void *arg) {

  sem_wait(&hammer);
  printf("Starting B1 (requires hammer) \n");
  sleep(1);
  printf("Finishing B1 \n");
  sem_post(&hammer);

  sem_wait(&hammer);
  sem_wait(&saw);
  printf("Starting B2 (requires hammer and saw) \n");
  sleep(1);
  printf("Finishing B2 \n");

  sem_post(&saw);
  sem_post(&hammer);

  return NULL;
}


int main(int argc, char **argv) {
  pthread_t a, b;
  int i;
  sem_init(&hammer, 0, 1);
  sem_init(&saw, 0, 1);


  pthread_create(&a, NULL, processA, NULL);
  pthread_create(&b, NULL, processB, NULL);

  pthread_join(a, NULL);
  pthread_join(b, NULL);


  printf("Everything finished.\n");
}
