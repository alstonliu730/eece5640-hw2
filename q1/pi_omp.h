#ifndef PI_OMP_H
#define PI_OMP_H

double CLOCK();

void intialize_thread_seed();
int get_rand();

int monte_carlo_pi();

double liebniz_sum(long i);
int liebniz_pi();

#endif /*PI_OMP_H*/