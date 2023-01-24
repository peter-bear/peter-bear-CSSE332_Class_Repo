/* Copyright 2021 Rose-Hulman */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

/**
  Imagine a group of friends are getting together to play music, but
  they are arriving at different times.  Arriving can happen at any
  time (e.g. when some other friends are playing).

  There are 3 different kinds of friends - drummers, singers, and
  guitarists.  It takes one of each kind to make a band, plus only
  1 band can be playing at once.  Once those conditions are met, the
  players can start playing and stop playing in any order.  However,
  all 3 players must stop playing before a new set of 3 can start
  playing.

  Example output:

  drummer arrived
  drummer arrived
  guitarist arrived
  guitarist arrived
  singer arrived
  drummer playing
  guitarist playing
  singer playing
  singer arrived
  singer arrived
  drummer arrived
  guitarist arrived
  drummer finished playing
  guitarist finished playing
  singer finished playing
  singer playing
  guitarist playing
  drummer playing
  singer finished playing
  guitarist finished playing
  drummer finished playing
  guitarist playing
  drummer playing
  singer playing
  guitarist finished playing
  drummer finished playing
  singer finished playing
  Everything finished.


 **/

int DRUM = 0;
int SING = 1;
int GUIT = 2;

char* names[] = {"drummer", "singer", "guitarist"};

int players[] = {0, 0, 0};
pthread_mutex_t locks[3];

pthread_mutex_t mutex_change = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t someone_arrived = PTHREAD_COND_INITIALIZER;

// because the code is similar, we'll just have one kind of thread
// and we'll pass its kind as a parameter
void* friend(void * kind_ptr) {
  int kind = *((int*) kind_ptr);
  printf("%s arrived\n", names[kind]);

  pthread_mutex_lock(&locks[kind]);
  pthread_mutex_lock(&mutex_change);
  players[kind]++;

  while(!(players[0]>0 && players[1]>0 && players[2]>0)){
    pthread_cond_wait(&someone_arrived, &mutex_change);
  }

 
  pthread_mutex_unlock(&mutex_change);
  pthread_cond_broadcast(&someone_arrived);

  printf("%s playing\n", names[kind]);
  sleep(1);

  pthread_mutex_lock(&mutex_change);
  players[kind]--;
  pthread_mutex_unlock(&mutex_change);

  printf("%s finished playing\n", names[kind]);
  

  pthread_mutex_unlock(&locks[kind]);
  return NULL;
}

pthread_t friends[100];
int friend_count = 0;

void create_friend(int* kind) {
  pthread_create(&friends[friend_count], NULL, friend, kind);
  friend_count++;
}

int main(int argc, char **argv) {
  for(int i=0; i<3;i++){
    pthread_mutex_init(&locks[i], NULL);
  }

  create_friend(&DRUM);
  create_friend(&DRUM);
  create_friend(&GUIT);
  create_friend(&GUIT);
  sleep(1);
  create_friend(&SING);
  create_friend(&SING);
  create_friend(&DRUM);
  create_friend(&GUIT);
  create_friend(&SING);

  // all threads must be created by this point
  // note if you didn't create an equal number of each, we'll be stuck forever
  for (int i = 0; i < friend_count; i++) {
    pthread_join(friends[i], NULL);
  }

  printf("Everything finished.\n");

}
