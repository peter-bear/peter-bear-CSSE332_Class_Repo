#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

// max number of values for each thread to sort
#define MAX_VALS_PER_THREAD 1000
// max threads allowed
#define MAX_N_SIZE 100


/* other global variable instantiations can go here */
struct arg_struct{
  int sort_type;
  int *num_array;
  int start_index;
  int length;
};

/* Uses a brute force method of sorting the input list. */
void BruteForceSort(int inputList[], int inputLength) {
  int i, j, temp;
  for (i = 0; i < inputLength; i++) {
    for (j = i+1; j < inputLength; j++) {
      if (inputList[j] < inputList[i]) {
        temp = inputList[j];
        inputList[j] = inputList[i];
        inputList[i] = temp;
      }
    }
  }
}

/* Uses the bubble sort method of sorting the input list. */
void BubbleSort(int inputList[], int inputLength) {
  char sorted = 0;
  int i, temp;
  while (!sorted) {
    sorted = 1;
    for (i = 1; i < inputLength; i++) {
      if (inputList[i] < inputList[i-1]) {
        sorted = 0;
        temp = inputList[i-1];
        inputList[i-1] = inputList[i];
        inputList[i] = temp;
      }
    }
  }
}

/* Merges two arrays.  Instead of having two arrays as input, it
 * merges positions in the overall array by re-ording data.  This 
 * saves space. */
void Merge(int *array, int left, int mid, int right) {
  int tempArray[MAX_VALS_PER_THREAD];
  int pos = 0, lpos = left, rpos = mid;
  while (lpos <= mid && rpos <= right) {
    if (array[lpos] < array[rpos]) {
      tempArray[pos++] = array[lpos++];
    } else {
      tempArray[pos++] = array[rpos++];
    }
  }
  while (lpos < mid)  tempArray[pos++] = array[lpos++];
  while (rpos <= right)tempArray[pos++] = array[rpos++];
  int iter;
  for (iter = 0; iter < pos; iter++) {
    array[iter+left] = tempArray[iter];
  }
  return;
}

/* Divides an array into halfs to merge together in order. */
void MergeSort(int array[], int inputLength) {
  int mid = inputLength/2;
  if (inputLength > 1) {
    MergeSort(array, mid);
    MergeSort(array + mid, inputLength - mid);
    // merge's last input is an inclusive index
    // printf("calling merge 0->%d, 1->%d\n mid %d\n",array[0], array[1], mid); 
    Merge(array, 0, mid, inputLength - 1);
  }
}

// you might want some globals, put them here

// here's a global I used you might find useful
char* descriptions[] = {"brute force","bubble","merge"};

// I wrote a function called thread dispatch which parses the thread
// parameters and calls the correct sorting function
//
// you can do it a different way but I think this is easiest
void* thread_dispatch(void* args) {
  
  struct arg_struct *data = (struct arg_struct *)args;
  
  printf("Sorting indexes %d-%d with %s\n", data->start_index, data->start_index+data->length-1, descriptions[data->sort_type]);
  sleep(1);

  struct timeval startt, stopt;
  suseconds_t usecs_passed;
  gettimeofday(&startt, NULL);
  
  // some code that takes time
  switch(data->sort_type){
    case 0:
      BruteForceSort(data->num_array, data->length);
      break;
    
    case 1:
      BubbleSort(data->num_array, data->length);
      break;
    
    case 2:
      MergeSort(data->num_array, data->length);
      break;
  }

  gettimeofday(&stopt, NULL);
  usecs_passed = stopt.tv_usec - startt.tv_usec;
  printf("Sorting indexes %d-%d with %s done in %ld usecs\n", data->start_index, data->start_index+data->length-1, descriptions[data->sort_type], usecs_passed);

  return (void *)usecs_passed;
}

int main(int argc, char** argv) {

  if(argc < 3) {
    printf("not enough arguments!\n");
    exit(1);
  }

  // I'm reading the value n (number of threads) for you off the
  // command line
  int n = atoi(argv[1]);
  if(n <= 0 || n > MAX_N_SIZE || n % 3 != 0) {
    printf("bad n value (number of threads) %d.  Must be a multiple of 3.\n", n);
    exit(1);
  }

  // I'm reading the number of values you want per thread
  // off the command line
  int vals_per_thread = atoi(argv[2]);
  if(vals_per_thread <= 0 || vals_per_thread > MAX_VALS_PER_THREAD) {
    printf("bad vals_per_thread value %d\n", vals_per_thread);
    exit(1);
  }

  int total_nums = n * vals_per_thread;
  int* data_array = malloc(sizeof(int) * total_nums);
  if(data_array == NULL) {
    perror("malloc failure");
    exit(1);
  }

  // initialize the test data for sort
  for(int i = 0; i < total_nums; i++) {
    // big reverse sorted list
    data_array[i] = total_nums - i;
    // the test would be more traditional if we used random
    // values, but this makes it easier for you to visually
    // inspect and ensure you're sorting everything
  }

  // create your threads here
  pthread_t threads[n];

  struct arg_struct* threads_data = malloc(sizeof(struct arg_struct) * n); 
  
  for(int i=0; i< n; i++){
    threads_data[i].sort_type = i%3;
    threads_data[i].num_array = &data_array[i*vals_per_thread];
    threads_data[i].start_index = i*vals_per_thread;
    threads_data[i].length = vals_per_thread;
    pthread_create(&threads[i], NULL, thread_dispatch, &threads_data[i]);
  }

  // wait for them to finish
  int brute_force[n/3];
  int bubble[n/3];
  int merge[n/3];
  int k =0;
  int m =0; 
  int l =0;
  for(int i=0;i<n;i++){
    int status;
    pthread_join(threads[i], (void**)&status);
    switch(i%3){
      case 0:
        brute_force[k] = status;
        k++;
        break;
      
      case 1:
        bubble[m] = status;
        m++;
        break;

      case 2:
        merge[l] = status;
        l++;
        break;
    }
  }
  MergeSort(brute_force, k);
  MergeSort(bubble, m);
  MergeSort(merge, l);

  

  // print out the algorithm summary statistics
  double sum = 0.0;
  for(int i=0;i<k;i++){
    sum += brute_force[i];
  }
  printf("%s avg %.6f min %d max %d\n", descriptions[0], sum/k, brute_force[0], brute_force[k-1]);

  sum = 0;
  for(int i=0;i<m;i++){
    sum += bubble[i];
  }
  printf("%s avg %.6f min %d max %d\n", descriptions[1], sum/m, bubble[0], bubble[m-1]);

  sum = 0;
  for(int i=0;i<l;i++){
    sum += merge[i];
  }
  printf("%s avg %.6f min %d max %d\n", descriptions[2], sum/l, merge[0], merge[l-1]);

  // print out the result array so you can see the sorting is working
  // you might want to comment this out if you're testing with large data sets
  printf("Result array:\n");
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < vals_per_thread; j++) {
      printf("%d ", data_array[i*vals_per_thread + j]);
    }
    printf("\n");
  }

  free(threads_data);
  free(data_array); // we should free what we malloc
}
