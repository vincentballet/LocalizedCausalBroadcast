import sys, os, math, signal
import numpy as np
from time import sleep, time

# Showing help if arguments are incorrect
if len(sys.argv) < 4:
    print("Usage: %s dir_with_da_proc m=500 n=5" % sys.argv[0])
    print(" The directory must contain the membership file and also the da_proc executable")
    sys.exit(0)

# original working directory
orig_dir = os.getcwd()

# messages to send
m = int(sys.argv[2])

# time to initialize
wait_time = 2

# Obtaining directory name
d = sys.argv[1]

# Going to that directory
os.chdir(d)

f = open('membership', 'w')
n = int(sys.argv[3])

print('Using %d messages and %d processes' % (m, n))

f.write('%d\n' % n)
for i in range(1, n + 1):
  f.write('%d 127.0.0.1 110%02d\n' % (i, i))
f.close()

# Reading membership file
membership = list(map(lambda x : x.split(), filter(lambda x: len(x) > 0, open('membership', 'r').read().split('\n'))))
n = int(membership[0][0])
membership = [t[1:] for t in membership[1:n+1]]

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
#for i in range(n)[1:]:
for i in range(n):
  pids += [os.spawnlp(os.P_NOWAIT, './da_proc', 'da_proc', str(i + 1), 'membership', str(m))]
#pids += [int(open('da_proc_1.pid', 'r').read())]

print("Created processes %s" % pids)

# sleeping until processes are ready
sleep(wait_time)

# starting processes
print("Starting processes")

# get start time
time_start = time()

for pid in pids:
  os.kill(pid, signal.SIGUSR2)

# waiting for processes...
while True:
  finished = True
  for i in range(1, n + 1):
    finished = finished and os.path.isfile('da_proc_%d.recvall' % i)
  if finished: break
  sleep(0.1)

# get end time
time_end = time()

# killing processes
for pid in pids:
  os.kill(pid, signal.SIGTERM)

# waiting to finish
sleep(wait_time)

# Reading logs
logs = {i: list(filter(lambda x : len(x) > 0, open('./da_proc_%d.out' % i, 'r').read().split('\n'))) for i in range(1, n + 1)}

# counting messages sent
messages = [[0 for y in range(n)] for x in range(n)]
for i in range(1, n + 1):
  for msg in logs[i]:
    if not msg.startswith('Destination'): continue
    msg = msg.split()
    dst, payload = int(msg[1]), int(msg[-1])
    messages[i - 1][dst - 1] += 1

messages = np.array([[messages[x][y] for y in range(n) if x != y] for x in range(n)]).flatten()
print('Sent messages: %.1f +- %.1f, had to send %d' % (np.mean(messages), np.std(messages), m * n))

# printing stats
time_delta = time_end - time_start
print("Sent %d messages over %d machines and received all in %.1f seconds, throughput = %.1f" % (m, n, time_delta, (m * n) / time_delta))

# also writing time to a file in ORIGINAL place
os.chdir(orig_dir)
open('time.out', 'w').write(str(time_delta) + '\n')
