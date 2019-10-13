#!/usr/env/bin python
import matplotlib
matplotlib.use('Agg')
matplotlib.rcParams['figure.dpi'] = 300
import matplotlib.pyplot as plt
from pathlib import Path
import numpy as np
import re

def full_time(outdir):
    result = []
    for trial in range(1, trial_num+1):
        output = next((outdir / str(trial)).glob("*.out"))
        with output.open('r') as f:
            for i in f:
                match = re.search("real\t0m(.*)s$", i)
                if match:
                    result.append(float(match.group(1)))
                    break
    return result

def get_single_experiment_result(outdir):
    result = []
    for trial in range(1, trial_num+1):
        output = next((outdir / str(trial)).glob("*.out"))
        with output.open('r') as f:
            for i in f:
                match = re.search("time.*: (.*)$", i)
                if match:
                    result.append(float(match.group(1)))
                    break
    return result

PLOTTING_DIR = Path('./plots')
PLOTTING_DIR.mkdir(exist_ok=True)

#real   0m52.406s
trial_num = 5
parallel_output_dir = list(Path('./parallel_output').glob('*'))
parallel_output_dir.sort(key=lambda x : float(x.name))
serial_output_dir = Path('./serial_output')
serial_output = np.array(get_single_experiment_result(serial_output_dir))

parallel_process_num = [int(i.name) for i in parallel_output_dir]
parallel_output = np.array([get_single_experiment_result(i) for i in parallel_output_dir])
r = np.array(full_time(serial_output_dir))
rp = np.mean(serial_output / r)
rs = 1 - rp
Sp = lambda p : 1/(rs + rp/p)
print("serial output:\n{0}\naverage time: {1}\n".format(r, r.mean()))
print("parallel output:\n{0}\naverage time: {1}\n".format(parallel_output, parallel_output.mean(axis=1)))
x = [1] + parallel_process_num
y = [r.mean()] + list(parallel_output.mean(axis=1))
actual_s = r.mean() / y


#plt.plot(x[1:], theo_s, label = 'theoretical speedup factor')

plt.plot(parallel_process_num, Sp(parallel_process_num), label='theoretical speedup factor')
print('theoretical speedup factor is:\n{}'.format(Sp(parallel_process_num)))
plt.plot(x, actual_s, label='actual speedup factor')
print("actual speedup: \nsp = {0}\n".format(actual_s))
plt.xlabel('processor number')
plt.ylabel('speedup factor')
plt.legend()
plt.savefig(PLOTTING_DIR / 'speedupfactor.png')
plt.clf()

plt.plot(x, r.mean()/x, label='linear time')
plt.plot(x, y, label='experiment time')
plt.legend(loc='best')
plt.xticks(x)
plt.xlabel('number of process')
plt.ylabel('time taken')
plt.title('serial vs parallel')
plt.savefig(PLOTTING_DIR / 'time.png')
