#!/bin/bash
# Example with 1 nodes, 1 processes each = 1 processes
#
# Set the number of nodes to use (max 20)
#SBATCH -N 1
#
# Set the number of processes per node (max 16)
#SBATCH -n 1
#
# Cpus (max 16)
#SBATCH --cpus-per-task=1
#

# Load the compiler and MPI library
module load openmpi-2.0/gcc

# Run the program
mpirun ./circuitSatisfiability 1
