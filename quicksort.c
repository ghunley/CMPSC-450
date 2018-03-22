#include "omp.h"
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/*
 *
 * * @brief Quicksort
 * * @param pf_input - pointer to input buffer (pre-allocated and populated)
 * * @param ui_len - number of elements in buffers
 *
 * */

int NUM_THREADS = 8;

void QuickSortSerial(float *pf_output, float *pf_input, unsigned int ui_len);
void quickSort(float *arr, int left, int right);
void QuickSortParallel(float *pf_output, float *pf_input, unsigned int ui_len);
void quickSort_parallel(float *arr, int left, int right);
void get_walltime(double *wcTime);

int main(int argc, char *argv[]) {
     int i_R = 1000;
     int i_N = 100000;

     // parse input parameters
           if(argc >= 2)
                i_R = atoi(argv[1]);
           if(argc >= 3)
                i_N = atoi(argv[2]);
           if(argc >= 4)
                NUM_THREADS = atoi(argv[3]);

     float *input_vals = (float*)malloc(i_N*sizeof(float));
     float *serialOutput = (float*)malloc(i_N*sizeof(float));
     float *parallelOutput = (float*)malloc(i_N*sizeof(float));
     double startTime, stopTime;

     // populate input array
     printf("Populating input array with random values...\n\n");
     srand(time(NULL));

     for(int i = 0; i < i_N; i++) {
          input_vals[i] = rand() % i_N;
     }

     // copy values to output arrays
     for(int i = 0; i < i_N; i++) {
          serialOutput[i] = input_vals[i];
          parallelOutput[i] = input_vals[i];
     }

     // sort with serial quick sort
     printf("Sorting values with serial quick sort...\n");
     get_walltime(&startTime);
     QuickSortSerial(serialOutput, input_vals, i_N);
     get_walltime(&stopTime);
     printf("Serial quick sort completed in approx. %f seconds.\n\n", stopTime-startTime);

     // sort with parallel quick sort
     printf("Sorting values with parallel quick sort...\n");
     get_walltime(&startTime);
     QuickSortParallel(parallelOutput, input_vals, i_N);
     get_walltime(&stopTime);
     printf("Parallel quick sort completed in approx. %f seconds.\n\n", stopTime-startTime);
        
     return 0;
}

void QuickSortSerial(float *pf_output, float *pf_input, unsigned int ui_len) {
     
     quickSort(pf_output, 0, ui_len-1);
}

void quickSort(float *arr, int left, int right) {
     int i = left, j = right;
     int pivot = arr[(left + right) / 2];
     int tmp;

     while(i <= j) {
          while(arr[i] < pivot)
               i++;
          while(arr[j] > pivot)
               j--;
          if(i <= j) {
               tmp = arr[i];
               arr[i] = arr[j];
               arr[j] = tmp;
               i++;
               j--;
          }
     }

     if(j > left)
          quickSort(arr, left, j);
     if(i < right)
          quickSort(arr, i, right);
}

void QuickSortParallel(float *pf_output, float *pf_input, unsigned int ui_len) {

     #pragma omp parallel num_threads(NUM_THREADS)
     {
          #pragma omp single nowait
          {
               quickSort_parallel(pf_output, 0, ui_len-1);
          }
     }
}

void quickSort_parallel(float *arr, int left, int right) {
     int i = left, j = right;
     int pivot = arr[(left + right) / 2];
     int tmp;

     while(i <= j) {
          while(arr[i] < pivot)
               i++;
          while(arr[j] > pivot)
               j--;
          if(i <= j) {
               tmp = arr[i];
               arr[i] = arr[j];
               arr[j] = tmp;
               i++;
               j--;
          }
     }

     if((right-left) < 1000) {
          if(j > left)
               quickSort_parallel(arr, left, j);
          if(i < right)
               quickSort_parallel(arr, i, right);
     }
     else {
          #pragma omp task
          {
               quickSort_parallel(arr, left, j);
          }

          #pragma omp task
          {
               quickSort_parallel(arr, i, right);
          }
     }
}

void get_walltime(double *wcTime) {
     struct timeval tp;

     gettimeofday(&tp, NULL);
     * wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}
