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
logs = {i: open(d + '/%d.log' % i, 'r').read().split('\n') for i in range(1, n + 1)}

# Printing how many log messages are in the dict
for key, value in logs.items():
    print("Process %d: %d messages" % (key, len(value)))

# how many messages should have been sent by each process?
expected_messages = 10
