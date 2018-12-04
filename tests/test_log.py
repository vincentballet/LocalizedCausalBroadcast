# input file
f = open('../da_proc_1.out').read().splitlines()

# number of threads and messages
n_thread, n_msg = [int(t) for t in f[0].split()]

print("Have %d threads and %d messages per thread" % (n_thread, n_msg))

# map thread -> messages
sent_by = {}

# loop over lines
for line in f[1:]:
  # parsing one line
  thread, n, message, x = line.split()

  # sanity check
  assert thread == "thread" and message == "message", "Input format is invalid"

  # converting to int
  n, x = int(n), int(x)

  # filling sent_by
  if n not in sent_by.keys(): sent_by[n] = []
  sent_by[n] += [x]

# sanity check: number of threads
assert sorted(list(sent_by.keys())) == list(range(n_thread)), "Have missing threads"

# checking that all messages were logged
for key, value in sent_by.items():
  assert value == range(n_msg), "Thread %d has missing messages" % key

print("Correct")
