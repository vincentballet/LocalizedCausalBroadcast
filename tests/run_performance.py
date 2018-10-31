import sys, os, math, signal
import numpy as np
from time import sleep, time

# Showing help if arguments are incorrect
if len(sys.argv) < 2:
    print("Usage: %s dir_with_da_proc" % sys.argv[0])
    print(" The directory must contain the membership file and also the da_proc executable")
    sys.exit(0)

# messages to send
m = 20

# time to initialize
wait_time = 5

# Obtaining directory name
d = sys.argv[1]

# Going to that directory
os.chdir(d)

# Reading membership file
membership = list(map(lambda x : x.split(), filter(lambda x: len(x) > 0, open('membership', 'r').read().split('\n')[1:])))

# Counting processes
n = len(membership)
print('There are %d processes' % n)


# writing down crashed processes
open('crashed.log', 'w').write('')

# array for PIDs
pids = []

# killing old processes
os.system("killall -9 da_proc")
os.system("rm *.recvall")

# creating processes
for i in range(n):
  pids += [os.spawnlp(os.P_NOWAIT, './da_proc', 'da_proc', str(i + 1), 'membership', str(m))]

print("Created processes %s" % pids)

# sleeping until processes are ready
sleep(wait_time)

# starting processes
print("Starting processes")

for pid in pids:
  os.kill(pid, signal.SIGUSR2)

# waiting for processes...
while True:
  finished = True
  for i in range(1, n + 1):
    finished = finished and os.path.isfile('da_proc_%d.recvall' % i)
  if finished: break
  sleep(0.1)

for pid in pids:
  os.kill(pid, signal.SIGINT)

sleep(wait_time)

print("All done")
