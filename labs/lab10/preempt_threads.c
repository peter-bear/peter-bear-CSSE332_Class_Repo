#include <malloc.h>
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "preempt_threads.h"

// 64kB stack
#define THREAD_STACK_SIZE 1024*64

/*
max number of threads

note if we had an expandable structure like an arraylist in C it
would be pretty easy to make this arbitrary, but let's not
introduce extra C libraries.

also note that the max we've picked is insanely small.  These threads
are very lightweight so we could probably have thousands.  But that
would make writing test code that exploits the thread max more
annoying.  So please leave this value as it is and use MAX_THREADS
(not the hardcorded value 5) in your code.
*/
#define MAX_THREADS 5

#define THREAD_UNFINISHED 0
#define THREAD_FINISHED 1
#define SLOT_AVAILABLE 0
#define SLOT_UNAVAILABLE 1

// storage for your thread data
ucontext_t threads[MAX_THREADS];
ucontext_t parent;

// add additional constants and globals here as you need
int cur_thread;
int thread_status[MAX_THREADS]; 
int thread_slots[MAX_THREADS];
int g_usec;

void block_alarm(){
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, SIGALRM);
    if(sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror ("sigprocmask");
    }
}

void unblock_alarm(){
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, SIGALRM);
    if(sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
        perror ("sigprocmask");
    }
}

void auto_yield() {
	swapcontext( &threads[cur_thread], &parent );
}


void catch_alarm(int sig_num)
{
    auto_yield();
    ualarm(g_usec,0);
}

void set_alarm(){
    signal(SIGALRM, catch_alarm);
}


void initialize_basic_threads() {
    block_alarm();

    for(int i=0; i<MAX_THREADS; i++){
        thread_status[i] = THREAD_FINISHED;
        thread_slots[i] = SLOT_AVAILABLE;
        threads[i].uc_stack.ss_sp = NULL;
    }

    g_usec = 5000;
}


void create_new_thread(void (*fun_ptr)()) {

    create_new_parameterized_thread(fun_ptr, NULL);

}


void auto_finish(void (*fun_ptr)(void*), void *parameter){

    ualarm(g_usec,0);

    unblock_alarm();
    fun_ptr(parameter);
    block_alarm();

    finish_thread();
}

void create_new_parameterized_thread(void (*fun_ptr)(void*), void* parameter) {
   
   block_alarm();

    void(*cast_ptr)() = (void(*)()) auto_finish;

    for(int i=0; i<MAX_THREADS; i++){
        if(thread_slots[i] == SLOT_AVAILABLE){
            getcontext( &threads[i]);
            threads[i].uc_link = 0;
            threads[i].uc_stack.ss_sp = malloc( THREAD_STACK_SIZE );
            threads[i].uc_stack.ss_size = THREAD_STACK_SIZE;
            threads[i].uc_stack.ss_flags = 0;

            if ( threads[i].uc_stack.ss_sp == 0 ) {
            perror( "malloc: Could not allocate stack\n" );
            exit (1);
            }

            makecontext(&threads[i], cast_ptr, 2, fun_ptr, parameter);
            thread_status[i] = THREAD_UNFINISHED;
            thread_slots[i] = SLOT_UNAVAILABLE;

            //printf("New thread created in slot %d\n", i);

            unblock_alarm();

            return;
        }
    }
    printf("No slots available");

    unblock_alarm();
}


void schedule_threads_with_preempt(int usecs){
    g_usec = usecs;
    bool is_all_cleaned = false;

    set_alarm();

    while(!is_all_cleaned){
        

        for(int i=0; i < MAX_THREADS; i++){
            block_alarm();
            if(thread_slots[i] == SLOT_UNAVAILABLE){
                cur_thread = i;
                if(thread_status[i] == THREAD_UNFINISHED){
                    swapcontext(&parent, &threads[cur_thread]);
                }
                else{
                    //printf("[Clean thread %d]\n", cur_thread);
                    thread_slots[cur_thread] = SLOT_AVAILABLE;
                    free(threads[cur_thread].uc_stack.ss_sp);
                }   
            }
        }

        is_all_cleaned = true;

        for(int i=0; i < MAX_THREADS; i++){
            if(thread_slots[i] == SLOT_UNAVAILABLE){
                is_all_cleaned = false;
            }
        }
    }

    ualarm(0,0);
    unblock_alarm();
}


void yield() {
    block_alarm();
    swapcontext(&threads[cur_thread], &parent);
}


void finish_thread() {

    //printf("Thread %d is finished\n", cur_thread);
    thread_status[cur_thread] = THREAD_FINISHED;

    swapcontext(&threads[cur_thread], &parent);

}
