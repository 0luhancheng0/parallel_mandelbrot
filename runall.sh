#!/bin/bash
sbatch serial.sh
parallelSbatch=./parallel.sh
for c in $(seq 2 2 10) 
do 
    sed -i "s/\(#SBATCH --ntasks=\)\([0-9]*\)/\1$c/g" $parallelSbatch
    sbatch $parallelSbatch
done 
