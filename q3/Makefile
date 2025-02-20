CC = gcc
CFLAGS = -Wall -Wextra -g -O2
MPFLAGS = -fopenmp

# Targets
TARGETS = color_graph

all: $(TARGETS)
	make $(TARGETS)

# Build the color graph
color_graph: color_graph.c color_graph.h
	$(CC) $(CFLAGS) -o $@ color_graph.c $(MPFLAGS)

clean: $(TARGETS)
	rm -f $(TARGETS)