from random import random

f = open("../membership", "w")

def print_write(s):
 """ Print and write to membership file """
 s = str(s)
 print(s)
 f.write(s + '\n')

# number of processes
n = 50

# printing number of processes
print_write(n)

# printing ports
for i in range(1, n + 1):
 print_write('%d 127.0.0.1 %d' % (i, 11000 + i))

# printing locality
for i in range(1, n + 1):
 # array with dependencies
 deps = []

 # with prob. 0.5 process will depend on another
 for j in range(1, n + 1):
  if random() > 0.5: deps += [str(j)]
 
 # printing locality 
 print_write('%d %s' % (i, ' '.join(deps)))
