# EECE 5640 HW 2
by Alston Liu

## Question 1
The pthread/OpenMP implementation of the pi approximation uses 2 methods:
- "Monte-Carlo Approximation"
- "Liebniz's Formula"

### Usage (pthread)
To build the pthread file, enter this command:
`make pi_pthread`

To run the pthread, here are the options: `./pi_pthread -p [Type] -t [# of Threads] -s [Sample Size]`

Here are the flag options:
- `-p`: Option to choose which approximation to use:
    - `0`: Monte-Carlo Approximation (Default)
    - `1`: Liebniz Approximation
- `-t`: Option to choose number of threads to use. (Default: 16 Threads)
- `-s`: Option to choose max number of points to use. (Default: 10,000)

### Usage (OpenMP)
To build the pthread file, enter this command:
`make pi_omp`

To run the pthread, here are the options: 
`./pi_omp -p [Type] -t [# of Threads] -s [Sample Size]`

Here are the flag options:
- `-p`: Option to choose which approximation to use:
    - `0`: Monte-Carlo Approximation (Default)
    - `1`: Liebniz Approximation
- `-t`: Option to choose number of threads to use. (Default: 16 Threads)
- `-s`: Option to choose max number of points to use. (Default: 10,000)

## Question 2
The Dining Philosopher's table is a concurrent algorithm problem. The main problem is to avoid deadlock, resource starvation or livelock.

### Usage
To build the file, enter this command:
- `make all`
- `make dining_ph`

To run the pthread, here are the options:
`./dining_ph [-s size] [-t threads] [-h]`

Here are the option flags:
- `-s`: Option to change the number of iteration
- `-t`: Option to change the number of threads/philosophers
- `-h`: Option to print help message

## Question 3
The Color Graphing problem uses a O(V * V) approach by using an adjacency matrix. To verify, it uses a O(V * V) for every iteration where there is an adjacent vertex that has the same color. However, since we are using threads to speedup the process. 

### Usage
To build the graph coloring program, use this command:
`make all`

To run this program, use this command:
`./color_graph [-t threads]`


