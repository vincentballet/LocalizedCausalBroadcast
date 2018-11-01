#!/usr/bin/python3 -u

import sys
from collections import defaultdict

i = 1
l = 1
nextMessage = defaultdict(lambda : 1)

with open(sys.argv[1]) as f:
    for line in f:
        tokens = line.split()
        
        # Check broadcast
        if tokens[0] == 'b':
            msg = int(tokens[1])
            if msg != i:
                print("Line {0}: Messages broadcast out of order. Expected message {1} but broadcast message {2}".format(l, i, msg))
                sys.exit(1)
            i += 1

        # Check delivery
        if tokens[0] == 'd':
            sender = int(tokens[1])
            msg = int(tokens[2])
            if msg != nextMessage[sender]:
                print("Line {0}: Message delivered out of order. Expected message {1}, but delivered message {2}".format(l, nextMessage[sender], msg))
                sys.exit(1)
            else:
                nextMessage[sender] = msg + 1

        # Increment line number counter
        l += 1
