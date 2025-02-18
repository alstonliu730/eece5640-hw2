CC = gcc
CFLAGS = -Wall -Wextra -g -O2
LDFLAGS = -lpthread
MPFLAGS = -fopenmp

# Targets
TARGETS = pi_pthread pi_omp

all: $(TARGETS)
	make $(TARGETS)

# Rules for building the pi_pthread executable
pi_pthread: pi_pthread.c pi_pthread.h
	$(CC) $(CFLAGS) -o $@ pi_pthread.c $(LDFLAGS)

# Rules for building the pi_omp executable
pi_omp: pi_omp.c pi_omp.h
	$(CC) $(CFLAGS) -o $@ pi_omp.c $(MPFLAGS)

# Clean up build artifacts
clean:
	rm -f $(TARGETS)