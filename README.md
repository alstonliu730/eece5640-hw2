# EECE 5640 HW 2
by Alston Liu

## Question 1
The pthread implementation of the pi approximation uses 2 methods:
- "Monte-Carlo Approximation"
- "Liebniz's Formula"

### Usage
To build the pthread file, enter this command:
`make pi_pthread`

To run the pthread, here are the options: `./pi_pthread -p [Type] -t [# of Threads] -s [Sample Size]`

Here are the flag options:
- `-p`: Option to choose which approximation to use:
    - `0`: Monte-Carlo Approximation (Default)
    - `1`: Liebniz Approximation
- `-t`: Option to choose number of threads to use. (Default: 16 Threads)
- `-s`: Option to choose max number of points to use. (Default: 10,000)

