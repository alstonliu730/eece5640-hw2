#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT(num)   (phnum + 4) % num
#define RIGHT(num)  (phnum + 1) % num

static int times = 12;

typedef struct {
    int id;                     // id of the philosopher
    int total_ph;               // total number of philosophers
    int* states;                // address of the state of each philosopher
    pthread_cond_t* cond_vars;    // address of the list of condvar of each philosopher
    pthread_mutex_t* cond_lock; // address of the mutex lock for the conditional variables
} ph_thread_t;

// initialize clock
double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec*1e-6);
}

// check if the fork is available
void test(ph_thread_t* ph, int phnum) {
    if (ph->states[LEFT(ph->total_ph)] != EATING && 
        ph->states[RIGHT(ph->total_ph)] != EATING && 
        ph->states[phnum] == HUNGRY) {
            ph->states[phnum] = EATING;
            pthread_cond_signal(&ph->cond_vars[phnum]);
        }
}

// take the fork function
void take_fork(ph_thread_t* ph, int phnum) {
    pthread_mutex_lock(ph->cond_lock);

    // change state to hungry
    ph->states[phnum] = HUNGRY;
    printf("Philosopher %d is Hungry\n", ph->id);

    // test for condition
    test(ph, phnum);

    // wait for the condition variable
    if (ph->states[phnum] != EATING) {
        pthread_cond_wait(&ph->cond_vars[phnum], ph->cond_lock);
    }
    printf("Philosopher %d is Eating\n", ph->id);

    pthread_mutex_unlock(ph->cond_lock);
}

// put the fork back function
void put_fork(ph_thread_t* ph, int phnum) {
    pthread_mutex_lock(ph->cond_lock);

    // Indicates philosopher is thinking
    ph->states[phnum] = THINKING;
    printf("Philosopher %d is Thinking\n", ph->id);

    // Test for condition
    test(ph, RIGHT(ph->total_ph));
    test(ph, LEFT(ph->total_ph));

    pthread_mutex_unlock(ph->cond_lock);
}

// philosopher's thread
void* philosophers(void* args) {
    // initialize thread arguments
    ph_thread_t* ph_arg = (ph_thread_t*) args;
    int id = ph_arg->id;

    int c = 0;
    while (c < times) {
        sleep(1); // think for 1 sec
        take_fork(ph_arg, id);
        sleep(0.5); // eat for 0.5 sec
        put_fork(ph_arg, id);
        c++;
    }

    return NULL;
}

// Dining philosopher fork.
int philosopher_forks(int nThreads) {
    int i;
    pthread_t threads_id[nThreads];
    double t1, total;

    // philosopher variables
    int ph_states[nThreads];              // list of philosopher states
    pthread_cond_t ph_conds[nThreads];    // conditional variables
    pthread_mutex_t ph_cond_lock = PTHREAD_MUTEX_INITIALIZER;
    
    // initialize conditional variables and states
    for(i = 0; i < nThreads; i++) {
        pthread_cond_init(&ph_conds[i], NULL);
        ph_states[i] = THINKING;
    }

    // initialize thread arguments in the stack
    ph_thread_t ph_args[nThreads];

    // Benchmark Start
    t1 = CLOCK();

    // initialize philosophers threads
    for(i = 0; i < nThreads; i++) {
        ph_args[i].id = i;
        ph_args[i].total_ph = nThreads;
        ph_args[i].states = ph_states;
        ph_args[i].cond_vars = ph_conds;
        ph_args[i].cond_lock = &ph_cond_lock;
        pthread_create(&threads_id[i], NULL, philosophers, &ph_args[i]);
    }

    // join their threads
    for(i = 0; i < nThreads; i++) {
        pthread_join(threads_id[i], NULL);
    }

    // Benchmark End
    total = CLOCK() - t1;

    // Result
    printf("Philosopher's Fork Time Elapsed: %0.3f\n", total);

    // clean up procedure
    for(i = 0; i < nThreads; i++) {
        pthread_cond_destroy(&ph_conds[i]);
    }
    pthread_mutex_destroy(&ph_cond_lock);

    return 0;
}

int main(int argc, char** argv) {
    int opt;    // option int
    int nThreads =  5; // default 5 philosophers

    // get user arguments
    while((opt = getopt(argc, argv, "t:s:h")) != -1) {
        int temp;
        switch (opt) {
            case 't':
                temp = atoi(optarg);
                if (temp < 0){
                    printf("Invalid input for Number of Philosophers (Threads). Default: %d\n", nThreads);
                } else {
                    nThreads = temp;
                }
                break;
            case 's':
                temp = atoi(optarg);
                if (temp < 12) {
                    printf("Sample size too small. Using default: %d\n", times);
                } else {
                    times = temp;
                }
                break;
            case 'h':
                printf("Usage: %s [-s size] [-t threads] [-h]\n", argv[0]);
                printf("  -s size     Set the sample size\n");
                printf("  -t threads  Set the number of threads/philosophers\n");
                printf("  -h          Display this help message\n");
                return 0;
            default:
                fprintf(stderr, "Usage: %s [-s size] [-t threads] [-h]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    philosopher_forks(nThreads);
    return 0;
}
