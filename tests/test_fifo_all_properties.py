import sys

if len(sys.argv) < 2:
    print("Usage: %s dir_with_log_files" % sys.argv[0])
    print(" The directory must contain the membership file and also files 1.log, ... N.log for each process")
    print(" The script prints last line CORRECT if all properties are satisfied")
    print(" The scripts prints last line INCORRECT with previous lines describing the issue if there were any")
    sys.exit(0)

d = sys.argv[1]

membership = map(lambda x : x.split(), filter(lambda x: len(x) > 0, open(d + '/membership', 'r').read().split('\n')[1:]))
n = len(membership)

print('There are %d processes' % n)
logs = {i: open(d + '/%d.log' % i, 'r').read().split('\n') for i in range(1, n + 1)}

for key, value in logs.items():
    print("Process %d: %d messages" % (key, len(value)))
