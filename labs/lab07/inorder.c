#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUM 4

int done[THREAD_NUM];
pthread_cond_t thread_condition[THREAD_NUM];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *thread(void *arg)
{
        int *num = (int *)arg;
        int index = *num -1;
        pthread_mutex_lock(&lock);
        if(index != 0){
                while(done[index-1] == 0 ){
                        pthread_cond_wait(&thread_condition[index-1], &lock);
                }
        }
        printf("%d wants to enter the critical section\n", *num);
        done[index] = 1;
        printf("%d is finished with the critical section\n", *num);
        pthread_cond_signal(&thread_condition[index]);
        pthread_mutex_unlock(&lock);
        return NULL;
}

int
main(int argc, char **argv)
{
        pthread_t threads[THREAD_NUM];
        int i;
        int nums[] = {2, 1, 4, 3};
        for(i = 0;i<THREAD_NUM;i++){
                done[i] = 0;
                thread_condition[i] = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
        }

        for(i = 0; i < THREAD_NUM; ++i) {
                pthread_create(&threads[i], NULL, thread, &nums[i]);

                if(i == 2)
                        sleep(3);
        }

        for(i = 0; i < THREAD_NUM; ++i) {
                pthread_join(threads[i], NULL);
        }

        printf("Everything finished\n");

        return 0;
}