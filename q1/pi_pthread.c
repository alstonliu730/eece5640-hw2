#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "pi_pthread.h"

// Monte Carlo Variables
double num_circle = 0.0;
double num_square = 0.0;
pthread_mutex_t circle_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t square_lock = PTHREAD_MUTEX_INITIALIZER;
int max_size = 0;

// thread local random seed
__thread unsigned int thread_seed;

// Liebniz Variables
double sum = 0.0;
pthread_mutex_t sum_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int lower;
    int upper;
    pthread_t th;
} liebniz_info;

// initialize clock
double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec*1e-6);
}

// initialize the random number generator for each thread
void intialize_thread_seed() {
    thread_seed = time(NULL) ^ pthread_self(); 
}

// returns a random number from 0 to RAND_MAX
int get_rand() {
    return rand_r(&thread_seed);
}

// monte-carlo thread method
void *monte_carlo_thread(void* args) {
    // receive size information for each thread
    int workload = *((int*) args);

    // keep track of local circle
    int local_num_circle = 0;

    // initialize thread seed
    intialize_thread_seed(); 
    
    // loop through the divided workload amount
    for(int i = 0; i < workload; i++) {
        // acquire lock for square
        pthread_mutex_lock(&square_lock);

        // increment square safely in mutex lock
        num_square++;

        // return lock for square
        pthread_mutex_unlock(&square_lock);

        // get random number
        double x = (double)(get_rand() % (max_size + 1)) / max_size;
        double y = (double)(get_rand() % (max_size + 1)) / max_size;

        // calculate the distance
        double d = x * x + y * y;

        // check if it's within the circle
        if (d <= 1.0) {
            local_num_circle++;
        }
    }

    // update number of points after workload
    pthread_mutex_lock(&circle_lock);
    num_circle += local_num_circle;
    // printf("Num. of Circle in Thread %ld: %0.2f\n", pthread_self(), num_circle);
    pthread_mutex_unlock(&circle_lock);
    
    return NULL;
}

// Monte Carlo Execution method
int monte_carlo_pi(int nThreads, int size) {
    printf("------ Monte Carlo Simulation ------\n");
    printf("Number of Threads: %d\n", nThreads);
    printf("Sample size: %d\n", size);
    // reset global variables
    max_size = size;
    num_circle = 0;
    num_square = 0;

    // create benchmark variables 
    double t1, t2, total;

    // split size work for each thread
    int workloads[nThreads];
    int base_workload = size / nThreads;
    int extra = size % nThreads;

    // Distribute work load
    for (int i = 0; i < nThreads; i++) {
        workloads[i] = base_workload + (i < extra ? 1 : 0);
    }

    // start the clock
    t1 = CLOCK();

    // create pthreads
    pthread_t pi_threads[nThreads];

    // spawn the pthreads
    for(int i = 0; i < nThreads; i++) {
        pthread_create(&pi_threads[i], NULL, monte_carlo_thread, &workloads[i]);
    }

    // wait for all threads to finish
    for(int i = 0; i < nThreads; i++) {
        pthread_join(pi_threads[i], NULL);
    }

    // calculate the result of the pi
    double pi = 4.0 * ((double) num_circle / (double) num_square);

    // stop the clock & find total time it took to finish
    t2 = CLOCK();
    total = t2 - t1;

    // print result
    printf("------ Monte Carlo Result ------\n");
    printf("Time Elapsed: %0.3f ms\n", total);
    printf("Points in Circle: %d\n", (int) (num_circle));
    printf("Points in Square: %d\n", (int) (num_square));
    printf("Estimation of Pi: %lf\n", pi);

    return 0;
}

double liebniz_sum(int i) {
    return (i % 2 == 0) ? (1.0 / (double) (2 * i + 1)) : (-1.0 / (double) (2 * i + 1));
}
// Liebniz Thread Method
void *liebniz_thread(void* args) {
    // receive size information for each thread
    liebniz_info* workload = (liebniz_info *) args;
    int lower = workload->lower;
    int upper = workload->upper;

    // keep track of local sum
    double local_sum = 0.0;

    // traverse through lower and upper bound
    for (int i = lower; i <= upper; i++) {
        local_sum += liebniz_sum(i);
    }

    pthread_mutex_lock(&sum_lock);
    sum += local_sum;
    pthread_mutex_unlock(&sum_lock);

    return NULL;
}

// Liebniz Execution Method
int liebniz_pi(int nThreads, int size) {
    printf("------ Liebniz Formula ------\n");
    printf("Number of Threads: %d\n", nThreads);
    printf("Sample size: %d\n", size);

    // create pthreads
    liebniz_info pi_threads[nThreads];

    int base_workload = size / nThreads;
    int extra = size % nThreads;
    int count = 0;

    // create benchmark variables 
    double t1, t2, total;

    // start the clock
    t1 = CLOCK();

    // spawn the pthreads
    for(int i = 0; i < nThreads; i++) {
        pi_threads[i].lower = count;
        count += (base_workload + (i < extra ? 1 : 0));
        pi_threads[i].upper = count - 1;
        pthread_create(&pi_threads[i].th, NULL, liebniz_thread, &pi_threads[i]);
    }

    // wait for all threads to finish
    for(int i = 0; i < nThreads; i++) {
        pthread_join(pi_threads[i].th, NULL);
    }

    // calculate pi
    double pi = 4.0 * sum;

    // stop the clock
    t2 = CLOCK();
    total = t2 - t1;

    // print result
    printf("------ Liebniz Result ------\n");
    printf("Time Elapsed: %0.3f ms\n", total);
    printf("Liebniz Sum: %lf\n", sum);
    printf("Estimation of Pi: %0.12lf\n", pi);

    
    return 0;
}

int main(int argc, char** argv) {
    int opt; 
    int numThreads = 16;        // default num of threads
    int part = 0;               // 0 - indicates approximation using monte-carlo method
                                // 1 - indicates approximation using Leibniz's method
    int sample_size = 100000;   // default sample size
    // get user arguments
    while((opt = getopt(argc, argv, "p:t:s:h")) != -1) {
        int temp;
        switch (opt) {
            case 'p':
                temp = atoi(optarg);
                if (temp >= 2 || temp < 0) {
                    printf("Invalid Input for Pi Approximation Method. Using Monte-Carlo as default method.\n");
                    break;
                } else { 
                    part = temp; 
                }
                break;
            case 's':
                temp = atoi(optarg);
                if (temp <= 0) {
                    printf("Invalid input for sample size. Using default: %d\n", sample_size);
                    break;
                } else {
                    sample_size = temp;
                }
                break;
            case 't':
                temp = atoi(optarg);
                if (temp <= 0) {
                    printf("Invalid input for number of threads. Using default: %d\n", numThreads);
                } else {
                    numThreads = temp;
                }
                break;
            case 'h':
                printf("Usage: %s [-s size] [-t threads] [-h]\n", argv[0]);
                printf("  -p method   Set the approximation method\n");
                printf("              (0 -> Monte-Carlo | 1 -> Liebniz)\n");
                printf("  -s size     Set the sample size\n");
                printf("  -t threads  Set the number of threads\n");
                printf("  -h          Display this help message\n");
                return 0;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-p method] [-s size] [-t threads] [-h]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    int res = 0;
    if (!part) {
        res = monte_carlo_pi(numThreads, sample_size);
    } else {
        res = liebniz_pi(numThreads, sample_size);
    }

    // Clean up procedure
    pthread_mutex_destroy(&circle_lock);
    pthread_mutex_destroy(&square_lock);
    pthread_mutex_destroy(&sum_lock);
    
    return res;
}