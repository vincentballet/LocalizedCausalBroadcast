import sys

# Showing help if arguments are incorrect
if len(sys.argv) < 2:
    print("Usage: %s dir_with_log_files" % sys.argv[0])
    print(" The directory must contain the membership file and also files 1.log, ... N.log for each process")
    print(" The script prints last line CORRECT if all properties are satisfied")
    print(" The scripts prints last line INCORRECT with previous lines describing the issue if there were any")
    sys.exit(0)

# Obtaining directory name
d = sys.argv[1]

# Reading membership file
membership = list(map(lambda x : x.split(), filter(lambda x: len(x) > 0, open(d + '/membership', 'r').read().split('\n'))))
n = int(membership[0][0])
membership = [t[1:] for t in membership[1:n+1]]

# Counting processes
n = len(membership)
print('There are %d processes' % n)

# reading list of crashed processes
crashed = list(map(int, open(d + '/crashed.log', 'r').read().split()))

# creating list of correct processes
correct = [x for x in range(1, n + 1) if x not in crashed]

# correct / crashed
print("Processes %s were correct and processes %s were crashed" % (correct, crashed))

# Reading logs
logs = {i: list(filter(lambda x : len(x) > 0, open(d + '/da_proc_%d.out' % i, 'r').read().split('\n'))) for i in range(1, n + 1)}

# Printing how many log messages are in the dict
for key, value in logs.items():
    print("Process %d: %d messages" % (key, len(value)))

# how many messages should have been sent by each process?
expected_messages = 10

# Were there errors?
were_errors = False

def soft_assert(condition, message = None):
    """ Print message if there was an error without exiting """
    global were_errors
    if not condition:
        if message:
            print("ASSERT failed " + message)
        were_errors = True

# messages broadcast by a process. idx -> array
broadcast_by = {i: [] for i in range(1, n + 1)}

# messages delivered by a process. idx -> array
delivered_by = {i: [] for i in range(1, n + 1)}

# messages delivered by a process. (idx, idx) -> array
delivered_by_from = {(i, j): [] for i in range(1, n + 1) for j in range(1, n + 1)}

# Filling in broadcast_by and delivered_by
for process in range(1, n + 1):
  for entry in logs[process]:
    if entry.startswith("b "):
      broadcast_by[process] += [int(entry[2:])]
    elif entry.startswith("d "):
      by = process
      from_ = int(entry.split()[1])
      content = int(entry.split()[2])
      delivered_by_from[(by, from_)] += [content]
      delivered_by[by] += [content]

# BEB1 Validity: If a correct process broadcasts a message m, then every correct process eventually delivers m.
for p in correct:
  for msg in broadcast_by[p]:
    for p1 in correct:
      soft_assert(msg in delivered_by[p1], "BEB1 Violated. Correct %d broadcasted %s and correct %d did not receive it" % (p, msg, p1))

# BEB2: No duplication
for p in range(1, n + 1):
  for s in range(1, n + 1):
    delivered_by_p_f_s = delivered_by_from[(p, s)]
    soft_assert(len(delivered_by_p_f_s) == len(set(delivered_by_p_f_s)), "BEB2 Violated. Process %d delivered some messages from %d twice" % (p, s))

# BEB3: No creation
for p in range(1, n + 1):
  for p1 in range(1, n + 1):
    sent = broadcast_by[p]
    delivered = delivered_by_from[(p1, p)]
    for msg in delivered:
      soft_assert(msg in sent, "BEB3 violated. Message %d was NOT send from %d and WAS delivered by %d" % (msg, p, p1))

# URB4: Agreement. If a message m is delivered by some (correct/faulty) process, then m is eventually delivered by every correct process.
all_delivered = [x for p in correct for x in delivered_by[p]]
for msg in all_delivered:
  delivered_all = [p for p in range(1, n + 1) if msg in delivered_by[p]]
  notdelivered_correct = [p for p in correct if msg not in delivered_by[p]]
  soft_assert(len(delivered_all) == 0 or len(notdelivered_correct) == 0, "URB4 Violated. Process %s delivered %d and correct %s did not deliver it" % (delivered_all, msg, notdelivered_correct))

# RB4 secondary check
for p in correct:
  delivered_by_p = delivered_by[p]
  for p1 in correct:
    delivered_by_p1 = delivered_by[p1]
    for msg in delivered_by_p:
      soft_assert(msg in delivered_by_p1)#, "RB4 Violated. Correct %d delivered %d and correct %d did not deliver it" % (p, msg, p1))

# FRB5: FIFO delivery: If some process broadcasts message m1 before it broadcasts message m2 , then no correct process delivers m2 unless it has already delivered m1
for p in range(1, n + 1):
  for i, msg1 in enumerate(broadcast_by[p]):
    for j, msg2 in enumerate(broadcast_by[p]):
      if i < j:
        for p1 in correct:
          del_p1 = delivered_by[p1]
          if msg1 in del_p1 and msg2 in del_p1:
            ind1 = del_p1.index(msg1)
            ind2 = del_p1.index(msg2)
            soft_assert(ind1 < ind2, "FRB5 violated: Process %d sent %d before %d and correct process %d delivered %d before %d" %
                                     (p, msg1, msg2, p1, msg2, msg1))

# printing the last line with status
print("INCORRECT" if were_errors else "CORRECT")
