#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1GB
#SBATCH --time=0-00:10:00
#SBATCH --array=1-5
#SBATCH --output=serial-%a.out
#SBATCH --partition=m3j
export LD_LIBRARY_PATH=$SLURM_SUBMIT_DIR/lib:$LD_LIBRARY_PATH
EXEC=$SLURM_SUBMIT_DIR/serial/mandelbrot
OUTDIR=$SLURM_SUBMIT_DIR/serial_output/$SLURM_ARRAY_TASK_ID
mkdir -p $OUTDIR
CONFIG=$OUTDIR/config.txt
lscpu > $CONFIG
env | grep SLURM >> $CONFIG
cd $OUTDIR
time srun ${EXEC}
mv $SLURM_SUBMIT_DIR/serial-${SLURM_ARRAY_TASK_ID}.out .
