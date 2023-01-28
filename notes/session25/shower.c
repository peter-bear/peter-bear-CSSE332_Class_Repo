
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdbool.h>

#define SHOWER_TIME 2

/*

  So these threads do their best thinking in the shower.

  Threads want to use the shower but only one thread can use the
  shower at once.  Once in the shower a thread showers for at least
  SHOWER_TIME.  Then the thread checks to see if any other threads
  want to use the shower, if yes the thread finishes in the shower and
  exits.

  If no other threads want to use the shower, the thread will stay in
  the shower and begin the process again.  This means that a thread
  will never leave the shower unless another thread arrives.

  Hint: this problem can actually be solved with just mutexes, but a
  cv solution is also good.

  Example output:

thread 1 wants to use the shower
thread 1 is using shower
thread 1 decides to stay in the shower a little longer
thread 3 wants to use the shower
thread 2 wants to use the shower
thread 4 wants to use the shower
thread 1 is finished with shower
thread 3 is using shower
thread 3 is finished with shower
thread 2 is using shower
thread 2 is finished with shower
thread 4 is using shower
thread 4 decides to stay in the shower a little longer
thread 4 decides to stay in the shower a little longer
<would run forever but main quits>

 */


 /*
  State of the world:
  1. shower occupy
  2. number of thread waiting

Shower:
  if occupy? wait go_to_shower

Waiting Area:
  if (number_of_thread == 0) (stay in shower) leave

 */

// set up state variables
int shower_occupied = 0;
int num_thread_waiting = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t door = PTHREAD_COND_INITIALIZER;

void *thread(void *arg)
{
    int num = (int) arg; //ignore the sketchy way I'm passing a parameter here
    printf("thread %d wants to use the shower\n", num);
    // I reccommend you leave the above print statement outside of
    // any lock, otherwise a certian kind of bug will be less obvious
    pthread_mutex_lock(&lock);
    num_thread_waiting++;
    while(shower_occupied){
        pthread_cond_wait(&door, &lock);
    }
    num_thread_waiting--;
    shower_occupied  = 1;
    pthread_mutex_unlock(&lock);

    // exactly one thread executing at this time

    printf("thread %d is using shower\n", num);
    while(1) {

        sleep(SHOWER_TIME);
        // anytime read or change the global variables, grab the lock
        pthread_mutex_lock(&lock);
        if(num_thread_waiting > 0){            
            shower_occupied = 0;
            pthread_mutex_unlock(&lock);

            pthread_cond_signal(&door);
            return NULL;
        }
        pthread_mutex_unlock(&lock);
        // if(your check to see if another thread is waiting) {
        //    printf("thread %d is finished with shower\n", num);
        //    other code
        //    break;
        // }

        printf("thread %d decides to stay in the shower a little longer\n", num);
        
    }
}



int main()
{
    
    pthread_t tid[100];
    int i = 0;

    
    pthread_create(&tid[i++],NULL, thread, (void*) 1);
    sleep(3);
    pthread_create(&tid[i++],NULL, thread, (void*) 2);
    pthread_create(&tid[i++],NULL, thread, (void*) 3);
    pthread_create(&tid[i++],NULL, thread, (void*) 4);
    sleep(10);

    //note that we don't wait for the last thread, otherwise this
    //program would never be done
    for(int j = 0; j < i - 1; j++) {
        pthread_join(tid[j],NULL);
    }


    return 0;
}
