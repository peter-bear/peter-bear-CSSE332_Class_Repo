/* Copyright 2016 Rose-Hulman Institute of Technology

   Here is some code that factors in a super dumb way.  We won't be
   attempting to improve the algorithm in this case (though that would be
   the correct thing to do).

   Modify the code so that it starts the specified number of threads and
   splits the computation among them.  You can be sure the max allowed 
   number of threads is 50.  Be sure your threads actually run in parallel.

   Your threads should each just print the factors they find, they don't
   need to communicate the factors to the original thread.

   ALSO - be sure to compile this code with -pthread or just used the 
   Makefile provided.

*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>

void *cal_factor(void* ptr){

  printf("running thread\n");
  int *data = (int *)ptr;
  int index = data[0];
  int start = data[1];
  int end = data[2];
  int target = data[3];


  for(int i=start; i<end; i++){
    printf("thread %d testing %d\n", index, i);
    if (target % i == 0) {
      printf("%d is a factor\n", i);
    }
  }
  pthread_exit(NULL);
}


int main(void) {
  /* you can ignore the linter warning about this */
  unsigned long long int target, i, start = 1;
  int numThreads;
  int dataNum = 4;
  printf("Give a number to factor.\n");
  scanf("%lld", &target);

  printf("How man threads should I create?\n");
  scanf("%d", &numThreads);
  if (numThreads > 50 || numThreads < 1) {
    printf("Bad number of threads!\n");
    return 0;
  }

  // for (i = 2; i <= target/2; i = i + 1) {
  //   /* You'll want to keep this testing line in.  Otherwise it goes so
  //      fast it can be hard to detect your code is running in
  //      parallel. Also test with a large number (i.e. > 3000) */
  //   printf("testing %llu\n", i);
  //   if (target % i == 0) {
  //     printf("%llu is a factor\n", i);
  //   }
  // }

  pthread_t threads[numThreads];
  int *threads_data = malloc((numThreads*dataNum)*sizeof(int));

  int part = (target-1)/numThreads;

  for(i=0; i<numThreads-1;i++){
    threads_data[i*dataNum] = i;
    threads_data[i*dataNum+1] = start;
    threads_data[i*dataNum+2] = start + part;
    threads_data[i*dataNum+3] = target;
    pthread_create(&threads[i], NULL, cal_factor, &threads_data[i*dataNum]);
    start += part;
  }

  threads_data[(numThreads-1)*dataNum] = (numThreads-1);
  threads_data[(numThreads-1)*dataNum+1] = start;
  threads_data[(numThreads-1)*dataNum+3] = target;
  if((target-1) % numThreads == 0){
    threads_data[(numThreads-1)*dataNum+2] = start + part+1;    
  }else{
    threads_data[(numThreads-1)*dataNum+2] = target + 1;
  }

  
  pthread_create(&threads[i], NULL, cal_factor, &threads_data[(numThreads-1)*dataNum]);

  for(i=0; i<numThreads;i++){  
    pthread_join(threads[i], NULL);
  }

  free(threads_data);

  return 0;
}

