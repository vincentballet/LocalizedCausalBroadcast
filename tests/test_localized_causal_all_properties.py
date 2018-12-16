import sys
import time

def vec_leq(vec1, vec2):
  """ Check that vec1 is less than vec2 elemtwise """
  assert isinstance(vec1, list), "Only work with lists"
  assert isinstance(vec2, list), "Only work with lists"
  assert len(vec1) == len(vec2), "Vector lengths should be the same"
  for x, y in zip(vec1, vec2):
    # if a coordinate is greater, returning false
    if x > y: return False

  # returning True if all xs are <= than ys
  return True

# small sanity check before starting the actual test
assert vec_leq([1,1,1], [1,1,1]) == True
assert vec_leq([1,0,1], [1,1,1]) == True
assert vec_leq([1,0,1], [0,1,1]) == False
assert vec_leq([0,0,1], [1,0,0]) == False

def soft_assert(condition, message = None):
    """ Print message if there was an error without exiting """
    global were_errors
    if not condition:
        if message:
            print("ASSERT failed " + message)
        were_errors = True

def main():
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
  membership = open(d + '/membership', 'r').read().splitlines()
  n = int(membership[0])
  dependencies = membership[n + 1:]
  dependencies = {i + 1: [int(t) for t in x.split()[1:]] for i, x in enumerate(dependencies)}
  membership = membership[1 : n + 1]
  membership = [x.split()[1:] for x in membership]

  # Counting processes
  n = len(membership)
  # print('There are %d processes' % n)

  # List of all processes
  processes = list(range(1, n + 1))

  # Printing dependencies
  for proc, deps in dependencies.items():
    pass
    # print("Process %d depends on {%s} (and itself)" % (proc, ', '.join([str(x) for x in deps])))

  # reading list of crashed processes
  crashed = list(map(int, open(d + '/crashed.log', 'r').read().split()))

  # creating list of correct processes
  correct = [x for x in processes if x not in crashed]

  # correct / crashed
  # print("Processes %s were correct and processes %s were crashed" % (correct, crashed))

  # Reading logs
  logs = {i: list(filter(lambda x : len(x) > 0, open(d + '/da_proc_%d.out' % i, 'r').read().split('\n'))) for i in processes}

  # Printing how many log messages are in the dict
  for key, value in logs.items():
    pass
      # print("Process %d: %d messages" % (key, len(value)))

  # how many messages should have been sent by each process?
  expected_messages = 10

  # messages broadcast by a process. idx -> array
  broadcast_by = {i: [] for i in processes}

  # messages delivered by a process. idx -> array
  delivered_by = {i: [] for i in processes}

  # messages delivered by a process. (idx, idx) -> array
  delivered_by_from = {(i, j): [] for i in processes for j in processes}

  # events (both deliveries and broadcasts)
  events = {i: [] for i in processes}

  # Filling in broadcast_by and delivered_by
  for process in processes:
    for entry in logs[process]:
      if entry.startswith("b "):
        content = int(entry[2:])
        broadcast_by[process] += [content]
        events[process] += [('b', process, content)]
      elif entry.startswith("d "):
        by = process
        from_ = int(entry.split()[1])
        content = int(entry.split()[2])
        delivered_by_from[(by, from_)] += [content]
        delivered_by[by] += [content]
        events[process] += [('d', from_, content)]

  # Sets for performance
  broadcast_by_set = {i: set(arr) for (i, arr) in broadcast_by.items()}
  delivered_by_set = {i: set(arr) for (i, arr) in delivered_by.items()}
  delivered_by_from_set = {(i, j): set(arr) for ((i, j), arr) in delivered_by_from.items()}

  start_time = time.time()
  # BEB1 Validity: If a correct process broadcasts a message m, then every correct process eventually delivers m.
  for p in correct:
    for msg in broadcast_by_set[p]:
      for p1 in correct:
        soft_assert(msg in delivered_by_set[p1], "BEB1 Violated. Correct %d broadcasted %s and correct %d did not receive it" % (p, msg, p1))
  print("BEB1 : {}".format(time.time() - start_time))
  start_time = time.time()

  # BEB2: No duplication
  for p in processes:
    for s in processes:
      delivered_by_p_f = delivered_by_from[(p, s)]
      soft_assert(len(delivered_by_p_f) == len(set(delivered_by_p_f)), "BEB2 Violated. Process %d delivered some messages from %d twice" % (p, s))
  print("BEB2 : {}".format(time.time() - start_time))
  start_time = time.time()
  
  # BEB3: No creation
  for p in processes:
    for p1 in processes:
      sent = broadcast_by_set[p]
      delivered = delivered_by_from_set[(p1, p)]
      for msg in delivered:
        soft_assert(msg in sent, "BEB3 violated. Message %d was NOT send from %d and WAS delivered by %d" % (msg, p, p1))
  print("BEB3 : {}".format(time.time() - start_time))
  start_time = time.time()

  # URB4: Agreement. If a message m is delivered by some (correct/faulty) process, then m is eventually delivered by every correct process.
  all_delivered = [x for p in correct for x in delivered_by_set[p]]
  for msg in all_delivered:
    delivered_all = [p for p in processes if msg in delivered_by_set[p]]
    notdelivered_correct = [p for p in correct if msg not in delivered_by_set[p]]
    soft_assert(len(delivered_all) == 0 or len(notdelivered_correct) == 0, "URB4 Violated. Process %s delivered %d and correct %s did not deliver it" % (delivered_all, msg, notdelivered_correct))
  print("URB4 : {}".format(time.time() - start_time))
  start_time = time.time()

  # RB4 secondary check
  for p in correct:
    delivered_by_p = delivered_by_set[p]
    for p1 in correct:
      delivered_by_p1 = delivered_by_set[p1]
      for msg in delivered_by_p:
        soft_assert(msg in delivered_by_p1)#, "RB4 Violated. Correct %d delivered %d and correct %d did not deliver it" % (p, msg, p1))
  print("RB4 : {}".format(time.time() - start_time))
  start_time = time.time()
  
  # CRB5: Causal delivery: For any message m1 that potentially caused a message m2, i.e., m1 -> m2 , no process delivers m2 unless it has already delivered m1.
  # (a) some process p broadcasts m1 before it broadcasts m2 ;
  # (b) some process p delivers m1 from a process (LOCALIZED) IT DEPENDS ON and subsequently broadcasts m2; or
  # (c) there exists some message m such that m1 -> m and m -> m2.
  # Process has dependencies dependencies[p] and itself

  # message dependencies: (sender, seq) -> [seq1, ..., seqN]
  msg_vc = {}

  # filling in vector clocks
  for p in processes:
    # current vector clock for a message (dependencies of a newly sent message)
    v_send = [0 for _ in range(n)]
    seqnum = 0

    # going over events
    for event in events[p]:
      # current message and type of event
      type_, msg = event[0], event[1:]

      # broadcast case: incrementing v_send
      if type_ == 'b':
        # copying v_send
        W = [x for x in v_send]

        # filling in seqnum
        W[p - 1] = seqnum

        # incrementing seqnum
        seqnum += 1

        # copying W to msg_vc
        msg_vc[msg] = [x for x in W]

      # delivery case: incrementing v_send if depend on the sender
      if type_ == 'd' and msg[0] in dependencies[p]:
        v_send[msg[0] - 1] += 1

  # PROPERTY TEST: for each process, for each delivery, must have W <= V_recv
  for p in processes:
    # currently delivered messages by process
    v_recv = [0 for _ in range(n)]

    # loop over events
    for event in events[p]:
      # only care about deliveries here
      if event[0] != 'd': continue

      # parsing message = (sender, seq)
      msg = event[1:]

      # sanity check
      assert msg in msg_vc, "Must have a vector clock for %s" % str(msg)

      # property test
      soft_assert(vec_leq(msg_vc[msg], v_recv), "CRB5 violated: Process %d have delivered %s with vector clock W = %s having V_recv = %s" % (p, str(msg), str(msg_vc[msg]), str(v_recv)))

      # incrementing v_recv
      v_recv[msg[0] - 1] += 1

  print("CRB5 : {}".format(time.time() - start_time))
  
  # printing the last line with status
  print("INCORRECT" if were_errors else "CORRECT")

if __name__ == '__main__':
  were_errors = False
  main()
