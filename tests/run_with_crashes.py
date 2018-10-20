import sys, os, math, signal
import numpy as np
from time import sleep, time

# Showing help if arguments are incorrect
if len(sys.argv) < 2:
    print("Usage: %s dir_with_da_proc" % sys.argv[0])
    print(" The directory must contain the membership file and also the da_proc executable")
    sys.exit(0)

# time to initialize
wait_time = 2

# maximal time of crash in ms
max_crash_time_ms = 2000

# maximal time to send stuff
max_send_time_ms = 5000

# Obtaining directory name
d = sys.argv[1]

# Going to that directory
os.chdir(d)

# Reading membership file
membership = map(lambda x : x.split(), filter(lambda x: len(x) > 0, open('membership', 'r').read().split('\n')[1:]))

# Counting processes
n = len(membership)
print('There are %d processes' % n)

# maximal number of crashed processes
max_crashed = n // 2

print("Crashing %d at most" % max_crashed)

# choosing how many processes to crash
n_crashed = np.random.choice(range(max_crashed + 1), p = [0.1, 0.5, 0.4])

# choosing which to crash
crashed_processes = np.random.choice(range(n), n_crashed, replace = False)

# choosing time to crash processes
crash_times = list(map(int, np.random.rand(n_crashed) * max_crash_time_ms))

print("Crashing %d processes: %s at times %s" % (n_crashed, crashed_processes, crash_times))

# array for PIDs
pids = []

# killing old processes
os.system("killall -9 da_proc")

# creating processes
for i in range(n):
  pids += [os.spawnlp(os.P_NOWAIT, './da_proc', 'da_proc', str(i + 1), 'membership')]

print("Created processes %s" % pids)

# sleeping until processes are ready
sleep(wait_time)

# starting processes
print("Starting processes")

for pid in pids:
  os.kill(pid, signal.SIGUSR1)

# was this process crashed already?
did_crash = []

# loop over milliseconds in time
start_time = 1000 * time()
while True:
    # current time difference
    delta = (1000 * time()) - start_time

    # checking if need to crash processes
    for i, process in enumerate(crashed_processes):
        # crash time for this process
        crash_time = crash_times[i]

        # crashing it if needed
        if delta >= crash_time and process not in did_crash:
            print("Crashing process %d" % (process + 1))
            did_crash += [process]
            os.kill(pids[process], signal.SIGINT)

    # exiting loop at the end
    if delta >= max_send_time_ms: break
            
    # sleep for one ms
    sleep(1e-3)

# stopping all processes
print("Stopping processes")
for pid in pids:
  os.kill(pid, signal.SIGTERM)

# waiting for log to be written
sleep(1)

print("All done")
