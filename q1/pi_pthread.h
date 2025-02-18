#ifndef PI_PTHREAD_H
#define PI_PTHREAD_H

# include <pthread.h>

// Extern declarations of global variables
// Monte-Carlo
extern double num_circle;
extern double num_square;
extern pthread_mutex_t circle_lock;
extern pthread_mutex_t square_lock;

// Liebniz
extern double sum;
extern pthread_mutex_t sum_lock;

// Functions
double CLOCK();

void intialize_thread_seed();
int get_rand();
void *monte_carlo_thread(void* arg);
int monte_carlo_pi(int nThreads, int size);

double liebniz_sum(int i);
void *liebniz_thread(void* args);
int liebniz_pi(int nThreads, int size);

#endif /*PI_PTHREAD_H*/