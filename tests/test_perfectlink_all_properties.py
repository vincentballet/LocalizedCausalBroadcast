import sys, os, math, signal
import numpy as np
from time import sleep, time

# Were there errors?
were_errors = False

def soft_assert(condition, message):
    """ Print message if there was an error without exiting """
    global were_errors
    if not condition:
        print("ASSERT failed " + message)
        were_errors = True
        
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

# Reading logs
logs = {i: list(filter(lambda x : len(x) > 0, open('./da_proc_%d.out' % i, 'r').read().split('\n'))) for i in range(1, n + 1)}


# Note : form of the logs is
# 1.log | pls   2 1 18 (dst = 2, src = 1, seq = 18)
# 1.log | plack 1 2 18 (dst = 1, src = 2, seq = 18)
# We want to see if those pairs exist

# Printing how many log messages are in the dict
for key, value in logs.items():
    print("Process %d: %d messages" % (key, len(value)))
    logs[key] = [l[2:] for l in value]
    logs[key] = [l.split() for l in logs[key]]

# ### No duplication - No message is delivered (to a process) more than once
for key, value in logs.items():
    logs_pld = list(filter(lambda l: l[0] == 'pld' in l, value))
    s = set([x for x in logs_pld if logs_pld.count(x) > 1])
    soft_assert(0 == len(s), "Some messages have been delived more than once : {}".format(s))

# ### No creation - No message is delivered unless it was sent
for key, value in logs.items():
    logs_pld = list(filter(lambda l: l[0] == 'pld' in l, value))
    for d in logs_pld:
        soft_assert(any(l[0] == 'pls' and l[2:] == d[1:] for l in logs[int(d[2])]), "Message {} is delivered while not sent ".format(d))
        # soft_assert(['pls', d[1], d[2], d[3]] in logs[int(d[2])], "Message {} is delivered while not sent ".format(d))
        
# ### Validity - If pi and pj are correct, then every message sent by pi to pj is eventually delivered by pj
for key, value in logs.items():
    logs_pls = list(filter(lambda l: l[0] == 'pls' in l, value))
    for m in logs_pls:
        soft_assert(['pld', d[1], d[2], d[3]] in logs[int(d[1])], "Message {} is never delivered".format(m))

# printing the last line with status
print("INCORRECT" if were_errors else "CORRECT")
