import sys


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

  # BEB1 Validity: If a correct process broadcasts a message m, then every correct process eventually delivers m.
  for p in correct:
    for msg in broadcast_by[p]:
      for p1 in correct:
        soft_assert(msg in delivered_by[p1], "BEB1 Violated. Correct %d broadcasted %s and correct %d did not receive it" % (p, msg, p1))

  # BEB2: No duplication
  for p in processes:
    for s in processes:
      delivered_by_p_f_s = delivered_by_from[(p, s)]
      soft_assert(len(delivered_by_p_f_s) == len(set(delivered_by_p_f_s)), "BEB2 Violated. Process %d delivered some messages from %d twice" % (p, s))

  # BEB3: No creation
  for p in processes:
    for p1 in processes:
      sent = broadcast_by[p]
      delivered = delivered_by_from[(p1, p)]
      for msg in delivered:
        soft_assert(msg in sent, "BEB3 violated. Message %d was NOT send from %d and WAS delivered by %d" % (msg, p, p1))

  # URB4: Agreement. If a message m is delivered by some (correct/faulty) process, then m is eventually delivered by every correct process.
  all_delivered = [x for p in correct for x in delivered_by[p]]
  for msg in all_delivered:
    delivered_all = [p for p in processes if msg in delivered_by[p]]
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
  for p in processes:
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

  # CRB5: Causal delivery: For any message m1 that potentially caused a message m2, i.e., m1 -> m2 , no process delivers m2 unless it has already delivered m1.
  # (a) some process p broadcasts m1 before it broadcasts m2 ;
  # (b) some process p delivers m1 from a process (LOCALIZED) IT DEPENDS ON and subsequently broadcasts m2; or
  # (c) there exists some message m such that m1 -> m and m -> m2.
  # Process has dependencies dependencies[p] and itself

  # message dependencies: (sender, seq) -> [(sender, seq), ..., (sender, seq)]
  # if a message has a dependency (sender, seq), it also has dependencies of that message
  msg_dep = {}

  # filling in one-hop dependencies
  for p in processes:
    # dependencies for messages at the moment of sending
    current_dependencies = []
    for event in events[p]:
      # current message and type of event
      type_, msg = event[0], event[1:]

      # adding message as a dependency
      soft_assert((msg not in current_dependencies) or msg[0] == p, "Error: broadcasted or delivered message %s twice!" % str(event))
      if msg not in current_dependencies:
        # (LOCALIZED) either it's my message or I depend on the process that has sent it
        if msg[0] == p or msg[0] in dependencies[p]:
          current_dependencies += [msg]

      # on broadcast, fill in message dependencies
      if type_ == 'b':
        soft_assert(msg not in msg_dep.keys(), "Error: broadcasted message %s twice!" % str(event))
        msg_dep[msg] = [x for x in current_dependencies]

  # performing BFS for each message, collecting all dependencies...
  # list of all messages
  all_msgs = sorted(list(msg_dep.keys()))

  # true dependencies for messages
  msg_dep_all = {}

  # loop over messages
  for msg in all_msgs:
    # messages which are visited
    deps = set()

    # queue for search (not bfs or dfs)
    queue = set()
    deps.add(msg)
    queue.add(msg)

    # while queue is not empty...
    while len(queue) > 0:
      # taking one element from the front
      elem = queue.pop()

      # going over dependencies
      for elem1 in msg_dep[elem]:
        # if it's not yet added
        if elem1 not in deps:
          # adding it to the queue and to dependencies
          queue.add(elem1)
          deps.add(elem1)

    # collecting all visited
    msg_dep_all[msg] = list(deps)

  # PROPERTY TEST: for each process, for each delivered message, must have already delivered its causal past
  for p in processes:
    # currently delivered by process
    delivered = set()

    # loop over events
    for event in events[p]:
      # only care about deliveries here
      if event[0] != 'd': continue

      # parsing message = (sender, seq)
      msg = event[1:]

      # adding as delivered
      delivered.add(msg)

      # for each message in past, must be past in delivered
      for past in msg_dep_all[msg]:
        soft_assert(past in delivered, "CRB5 Violated: message %s is causal past of message %s and it was not delivered before by process %d" % (str(past), str(msg), p))
  # printing the last line with status
  print("INCORRECT" if were_errors else "CORRECT")

if __name__ == '__main__':
  were_errors = False
  main()