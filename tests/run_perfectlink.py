import sys, os, math, signal
import numpy as np
from time import sleep, time
from random import shuffle

# Showing help if arguments are incorrect
if len(sys.argv) < 2:
    print("Usage: %s dir_with_da_proc" % sys.argv[0])
    print(" The directory must contain the membership file and also the da_proc executable")
    sys.exit(0)

# Obtaining directory name
d = sys.argv[1]

# Going to that directory
os.chdir(d)

# Reading membership file
membership = list(map(lambda x : x.split(), filter(lambda x: len(x) > 0, open('membership', 'r').read().split('\n'))))
n = int(membership[0][0])
membership = [t[1:] for t in membership[1:n+1]]

# Counting processes
n = len(list(membership))
print('There are %d processes' % n)

# array for PIDs
pids = []

# killing old processes
os.system("killall -9 da_proc")

# creating processes
rng = list(range(n))
shuffle(rng)
for i in rng:
  pids += [os.spawnlp(os.P_NOWAIT, './da_proc', 'da_proc', str(i + 1), 'membership', '10', 'test')]

sleep(4)
