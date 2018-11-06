import os
import numpy as np
import itertools
import time
from subprocess import call
import pandas as pd
import subprocess

def generate_perfectlinkh(t, m):
    with open("perfectlink_config_backup.cpp", "r", encoding="utf-8") as b, open("perfectlink_config.cpp", "w", encoding="utf-8") as f:
        for line in b:
            if 'TIMEOUT_MSG' in line:
                f.write('const unsigned PerfectLink::TIMEOUT_MSG = {};\n'.format(t))
            elif 'MAX_IN_QUEUE' in line:
                f.write('const unsigned PerfectLink::MAX_IN_QUEUE = {};\n'.format(m))
            else :
                f.write(line)

def main():
    # timeouts = np.linspace(50, 2000, 10)
    # maxinqueues = np.linspace(20, 100, 5)
    timeouts = [500, 1000]
    maxinqueues = [50]
    m = 20
    n = 3
    runs = 3
    run_times = []
    columns = ['TimeOut', 'MaxInQueue', 't1', 't2', 't3', 't_mean']
    df = pd.DataFrame(columns=columns)
    tot_runs = len(timeouts) * len(maxinqueues)

    for idx, x in enumerate(itertools.product(timeouts, maxinqueues)):
        print("{}/{} Running for TimeOut={}, MaxInQueue={}".format(idx+1, tot_runs, x[0], x[1])) 
        generate_perfectlinkh(x[0], x[1])
        call(["make -j4"])
        run_times = []
        for i in range(runs):
            print("\tRun " + str(i))
            call(["python", "tests/run_performance.py", ".", str(m), str(n)])
            with open("time.out", "r") as f:
                run_times.append(float(f.readline()))
        df.loc[idx] = [x[0], x[1], run_times[0], run_times[1], run_times[2], np.mean(run_times)]

    df.to_csv('results_hs.csv')

if __name__ == '__main__':
    main()