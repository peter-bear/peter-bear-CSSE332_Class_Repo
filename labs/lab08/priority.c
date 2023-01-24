#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond_finish = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int current;
int threads_finish[7];

/**
  In this system there are threads numbered 1-6 and a critical
  section.  The thread numbers are priorities, where thread 6 is
  highest priority, thread 5 is next, etc.

  If the critical section is empty, any thread can enter.  While a
  thread is in the critical section no other threads can enter -
  regardless of priority. When a thread leaves the critical section
  and there are threads waiting, the highest priority waiting thread
  is allowed to enter the critical section.

  Be sure a newly arriving thread can't jump into the critical
  section as the current thread finishes, bypassing priority rules.
  Solve this problem with mutexes/condition variables
 **/

int find_max(){
	for(int i=6; i>=1;i--){
		if(threads_finish[i] == 0 && i != current){
			return i;
		}
	}

	return -1;
}


void *thread(void *arg)
{
  int *num = (int *) arg;
  printf("%d wants to enter the critical section\n", *num);
  threads_finish[*num] = 0;
	pthread_mutex_lock(&mutex);
  if(current == -1){
    printf("%d has entered the critical section\n", *num);
    sleep(1);
    printf("%d is finished with the critical section\n", *num);
    
    
    threads_finish[*num] = 1;
		pthread_mutex_unlock(&mutex);
    
    return NULL;
  }

  while(*num != find_max()){
		pthread_cond_wait(&cond_finish, &mutex);
	}

  
	current = *num;
	printf("%d has entered the critical section\n", *num);
	sleep(1);
	printf("%d is finished with the critical section\n", *num);
	threads_finish[*num] = 1;
	pthread_cond_broadcast(&cond_finish);
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int
main(int argc, char **argv)
{
  int i;
  pthread_t threads[6];
  int nums[] = {2, 1, 4, 3, 5, 6};


  for(i = 0; i < 6; i++) {
    pthread_create(&threads[i], NULL, thread, &nums[i]);

    if(i == 2) sleep(10);
  }

  for(i = 0; i < 6; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Everything finished.\n");
}
