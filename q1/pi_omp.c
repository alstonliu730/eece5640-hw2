#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>

static long num_trials = 1000000;

// thread local random seed
__thread unsigned int thread_seed;

// initialize the random number generator for each thread
void intialize_thread_seed() {
    thread_seed = time(NULL) ^ (unsigned int) omp_get_thread_num(); 
}

// returns a random number from 0 to RAND_MAX
int get_rand() {
    return rand_r(&thread_seed);
}

// initialize clock
double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec*1e-6);
}

int monte_carlo_pi() {
    long i; long num_circle = 0;
    double pi, x, y, dist;
    double t1, total;

    t1 = CLOCK();
    #pragma omp parallel
    {   
        intialize_thread_seed();

        #pragma omp single
          printf(" %d threads \n",omp_get_num_threads());
        
        #pragma omp for reduction(+:num_circle) private(x,y,dist)
        for(i = 0; i < num_trials; i++) {
            x = ((double) (get_rand() % (num_trials + 1))) / (double) num_trials;
            y = ((double) (get_rand() % (num_trials + 1))) / (double) num_trials;

            dist = x*x + y*y;

            if (dist <= 1.0) {
                num_circle++;
            }
        }
    }

    // get pi estimation
    pi = 4.0 * ((double)num_circle / (double)num_trials);
    
    // benchmark
    total = CLOCK() - t1;

    // print result
    printf("------ Monte Carlo Result ------\n");
    printf("Time Elapsed: %0.3f ms\n", total);
    printf("Points in Circle: %ld\n", num_circle);
    printf("Estimation of Pi: %lf\n", pi);

    return 0;
}

double liebniz_sum(long i) {
    return (i % 2 == 0) ? (1.0 / (double) (2 * i + 1)) : (-1.0 / (double) (2 * i + 1));
}

int liebniz_pi() {
    long i;
    double pi, sum = 0.0;
    double t1, total;

    // begin benchmark
    t1 = CLOCK();
    #pragma omp parallel 
    {
        #pragma omp single
        printf(" %d threads \n",omp_get_num_threads());

        #pragma omp for reduction(+:sum)
        for(i=0; i < num_trials; i++) {
            sum += liebniz_sum(i);
        }
    }

    // calculate pi approximation
    pi = (double) 4.0 * sum;

    // benchmark end
    total = CLOCK() - t1;

    // print result
    printf("------ Liebniz Result ------\n");
    printf("Time Elapsed: %0.3f ms\n", total);
    printf("Liebniz Sum: %lf\n", sum);
    printf("Estimation of Pi: %0.12lf\n", pi);

    return 0;
}

int main(int argc, char** argv) {
    int opt; 
    int part = 0;               // 0 - indicates approximation using monte-carlo method
                                // 1 - indicates approximation using Leibniz's method
    // get user arguments
    while((opt = getopt(argc, argv, "p:t:s:h")) != -1) {
        int temp;
        switch (opt) {
            case 'p':
                temp = atoi(optarg);
                if (temp >= 2 || temp < 0) {
                    printf("Invalid Input for Pi Approximation Method. Using Monte-Carlo as default method.\n");
                } else { 
                    part = temp; 
                }
                break;
            case 's':
                temp = atol(optarg);
                if (temp <= 0) {
                    printf("Invalid input for sample size. Using default: %ld\n", num_trials);
                } else {
                    num_trials = temp;
                }
                break;
            case 't':
                temp = atoi(optarg);
                if (temp <= 0) {
                    printf("Invalid input for number of threads.\n");
                } else {
                    omp_set_num_threads(temp);
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

    if (part) {
        return liebniz_pi();
    } else {
        return monte_carlo_pi();
    }
}