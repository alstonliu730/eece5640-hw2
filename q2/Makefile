CC = gcc
CFLAGS = -Wall -Wextra -g -O2
LDFLAGS = -lpthread

# Targets
TARGETS = dining_ph

all: $(TARGETS)
	make $(TARGETS)

# Build the dining philosopher's fork
dining_ph: dining_phil.c dining_phil.h
	$(CC) $(CFLAGS) -o $@ dining_phil.c $(LDFLAGS)
