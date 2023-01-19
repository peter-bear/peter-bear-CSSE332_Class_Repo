#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int on_working_num = 0;
pthread_cond_t thread_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void *thread(void *arg)
{
	char *letter = (char *)arg;
	pthread_mutex_lock(&lock);
	while(on_working_num >= 3){
		pthread_cond_wait(&thread_condition, &lock);
	}
	on_working_num++;
	pthread_mutex_unlock(&lock);
	printf("%c wants to enter the critical section\n", *letter);

	printf("%c is in the critical section\n", *letter);
	sleep(1);
	printf("%c has left the critical section\n", *letter);
	on_working_num--;
	pthread_cond_signal(&thread_condition);
	return NULL;
}

int
main(int argc, char **argv)
{
	pthread_t threads[8];
	int i;
	char *letters = "abcdefgh";

	for(i = 0; i < 8; ++i) {
		pthread_create(&threads[i], NULL, thread, &letters[i]);

		if(i == 4)
			sleep(4);
	}

	for(i = 0; i < 8; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("Everything finished...\n");

	return 0;
}
