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
membership = map(lambda x : x.split(), filter(lambda x: len(x) > 0, open(d + '/membership', 'r').read().split('\n')[1:]))

# Counting processes
n = len(membership)
print('There are %d processes' % n)

# Reading logs
logs = {i: filter(lambda x : len(x) > 0, open(d + '/%d.log' % i, 'r').read().split('\n')) for i in range(1, n + 1)}

# Printing how many log messages are in the dict
for key, value in logs.items():
    print("Process %d: %d messages" % (key, len(value)))

# how many messages should have been sent by each process?
expected_messages = 10

# messages broadcast by a process. idx -> array
broadcast_by = {i: [] for i in range(1, n + 1)}

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

# Were there errors?
were_errors = False

def soft_assert(condition, message):
    """ Print message if there was an error without exiting """
    global were_errors
    if not condition:
        print("ASSERT failed " + message)
        were_errors = True

# Checking that each process has sent its messages
for process in range(1, n + 1):
  soft_assert(broadcast_by[process] == range(expected_messages), "Process %d should send all messages" % process)

# Checking if messages are correct
for dst in range(1, n + 1):
  for src in range(1, n + 1):
    soft_assert(delivered_by_from[(dst, src)] == range(expected_messages), "Process %d should receive all messages in correct order from %d; Got array %s" % (dst, src, str(delivered_by_from[(dst, src)])))

# printing the last line with status
print("INCORRECT" if were_errors else "CORRECT")
