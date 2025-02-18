#ifndef DINING_PHIL_H
#define DINING_PHIL_H

double CLOCK();

void test(ph_thread_t* ph, int phnum);
void put_fork(ph_thread_t* ph, int phnum);
void take_fork(ph_thread_t* ph, int phnum);

void* philosophers(void* args);
int philosopher_forks(int nThreads);

#endif /*DINING_PHIL_H*/