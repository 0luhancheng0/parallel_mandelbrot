#!/bin/bash
#SBATCH --ntasks=10
#SBATCH --mem-per-cpu=1GB
#SBATCH --cpus-per-task=1
#SBATCH --time=00:02:00
#SBATCH --array=1-5
#SBATCH --partition=m3j
#SBATCH --output=parallel-%A-%a.out
#SBATCH --nodes=1
hostname
module load openmpi
EXEC=$SLURM_SUBMIT_DIR/parallel/mandelbrot
OUTDIR=$SLURM_SUBMIT_DIR/parallel_output/$SLURM_NTASKS/$SLURM_ARRAY_TASK_ID
mkdir -p $OUTDIR
CONFIG=$OUTDIR/config.txt
lscpu > $CONFIG
env | grep SLURM >> $CONFIG
cd $OUTDIR
mpirun -n $SLURM_NTASKS ${EXEC}
mv $SLURM_SUBMIT_DIR/parallel-${SLURM_ARRAY_JOB_ID}-${SLURM_ARRAY_TASK_ID}.out .

