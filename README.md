### Localized Causal and FIFO Uniform Reliable Broadcast
Implementation of Localized Causal Broadcast and FIFO broadcast using UDP sockets in C++ for the CS-451 Distributed Algorithms [1] course at the Swiss Federal Institute of Technology in Lausanne (EPFL)

This project implements Perfect Links on top of UDP sockets, Best Effort Broadcast, Uniform Reliable Broadcast, Uniform FIFO Broadcast, Uniform Localized Causal Broadcast. The definitions of all abstractions are present in [2], and Localized Causal Broadcast is defined in <a href="https://github.com/sergeivolodin/LocalizedCausalBroadcast/blob/master/lcb_algo/main.pdf">lcb_algo/main.pdf</a>. The code has comments and competitive performance, see the `code/` folder.

2018, Vincent Ballet and Sergei Volodin

### Compilation
1. Build essentials (g++, make) are required. Build was tested on Linux and Mac OSX
2. Run `make -j4`

### Performance
Tested on a Raspberry Pi 3 (4 cores) on loopback interface. Lossy network corresponds to `netem delay 50ms 200ms loss 10% 25% reorder 25% 50%`
#### FIFO and LCB for number of processes n=5 and normal network, different number of messages m
<img src="https://github.com/sergeivolodin/LocalizedCausalBroadcast/raw/master/tests/figures/FIFO_M_N5.png" /> <img src="https://github.com/sergeivolodin/LocalizedCausalBroadcast/raw/master/tests/figures/LCB_M_N5.png" />

### FIFO and LCB for n=5 and lossy network, different number of messages m 
<img src="https://github.com/sergeivolodin/LocalizedCausalBroadcast/raw/master/tests/figures/FIFO_M_N5_lossy.png" /> <img src="https://github.com/sergeivolodin/LocalizedCausalBroadcast/raw/master/tests/figures/LCB_M_N5_lossy.png" />

### URB and LCB for fixed m and different n
<img src="https://github.com/sergeivolodin/LocalizedCausalBroadcast/raw/master/tests/figures/URB_N.png" /> <img src="https://github.com/sergeivolodin/LocalizedCausalBroadcast/raw/master/tests/figures/LCB_N.png" />

### Implementation details
1. Point-to-point links are subclassed from `Sender` and `Receiver` classes. A `Sender` is any object which can send data somewhere. A `Receiver` is an object containing links to many `Target` objects, and each time `Receiver` gets data it relays it to all targets.
2. UDP primitivies are implemented as `UDPSender` and `UDPReceiver` classes. Receiver object starts a loop in a thread which listens to all incoming messages and processes them in that thread (relays data to connected `Target` objects). This level of abstraction adds additional first 4 bytes to the data which indicate the sender process ID.
3. `ThreadedReceiver` is an abstraction which processes all received messages in a separate thread and allows for the `deliverToAll` to return immediately. Basically it implements an additional infinite buffer. `ThreadedSender` does the same for the `Sender` class.
4. `PerfectLink` is a class containing a `Sender` and a `Receiver` and implementing retransmissions on top of a possibly faulty link. It has a window size (parameter `MAX_IN_QUEUE`) and if all messages from the window were not acknowledged, it blocks the sender. Messages without acknowledgements are retransmitted each `TIMEOUT_MSG` milliseconds. We note that a `PerfectLink` uses a `ThreadedReceiver` meaning that even one of the targets of the `PerfectLink` blocks on delivery of a message, the link will continue to operate
5. `Broadcast` is an abstract class with `broadcast` method for broadcasting and `onMessage` method which would be called each time a message arrives.
6. `BestEffortBroadcast` is a `Broadcast` simply relaying the message to all targets. We note that it adds additional 4 bytes at the beginning which indicate the logical sender. Each message is broadcast to all other processes.
7. `UniformReliableBroadcast` implements URB with an assumption of correct majority of processes without any timing assumptions (without a failure detector). We note that each process relays each message it receives, so a single message sent from one machine will result in total of O(N^2) messages in the network. We achieve optimal performance by recomputing the number of ACKs a message has at each time we receive a new ACK. Therefore the performance is linear in number of messages: O(MN^2)
8. `FIFOBroadcast` is a performant implementation of FIFO. A queued for delivery messages are sorted by their sequence numbers, thus amortized complexity is O(logM) for each message (compared to O(1) for a trivial case). It adds the sender sequence number at the beginning of the message as first 4 bytes.
9. `LocalizedCausalBroadcast` is a performant blocking (waiting) implementation of LCB using vector clocks. Localized means that broadcast cares only about specific dependencies of a process provided my `Membership`, see <a href="https://github.com/sergeivolodin/LocalizedCausalBroadcast/blob/master/lcb_algo/main.pdf">lcb_algo/main.pdf</a>. This file also provides pseudocode of the algorithm implemented. Implemented version is also optimized: it uses N queues for messages (for each sender), and sorts them by sender sequence number and thus achieves performance of O(NlogM) for each arriving message. It adds 4N bytes at the beginning filled with vector clocks.
10. `InMemoryLog` implements a performant logger which first stores all debug information in memory, and only occasionally dumps data to a file
11. `Membership` provides number of processes and their addresses, and also the locality information.
12. `SeqTarget` prints each delivered message and is a `Target`
13. `test.cpp` contains code tests
14. `main.cpp` reads membership file (see Usage by running the program `da_proc`), waits for SIGUSR2 and starts to broadcast messages.

### Tests
See tests/
1. `lossy.sh` configures a lossy loopback interface for UDP and ICMP traffic
2. `test_log.py` and `test_log.cpp` tests log performance (corresponds to log test in `test.cpp`). `run_perfectlink.py` runs the Perfect Link code
3. `run_performance.py` will run processes without crashes and measure the total time before all messages are delivered. Must enable `DEBUG_FILES` in `common.h` and recompile code.
4. `get_time.py` will run `run_performance` and time the execution
5. `run_with_crashes.py` runs processes with random pauses and crashes at most a minority of processes
6. `hyperparam_search.ipynb` and `hyperparam_search.py` and `tradeoff_timeout_windowsize.ipynb` searches for PerfectLink hyperparameters to minimize total transmission time
7. `test_fifo_all_properties.py` will test the FIFO properties based on log files. Crashed processes are read from `../crashed.log`. `test_perfectlink_all_properties.py` does the same for perfect link. `test_localized_causal_all_properties.py` does the same for LCB.
8. `timing.ipynb` and `timing_lcb.ipynb` contain performance analysis results
9. `tests_for_tests` contain tricky cases for tests themselves

### Configuration
See `common.h` file for macro definitions:
```
/// @macro Enable localized causal broadcast
#define LOCALIZED_CAUSAL_BROADCAST
/// @macro Enable tests for main()
//#define DEBUG_TEST 1
/// @macro Output debug files (.timestamps and .recvall)
//#define DEBUG_FILES 1
/// @macro Message = 1000 * sender + payload
//#define UNIQUE_MESSAGES 1
/** @macro Print messages stored in memory */
//#define INMEMORY_PRINT 1
/// @macro Dump to a separate file immediately
//#define IMMEDIATE_FILE 1
/// @macro Debug perfectlink
//#define PERFECTLINK_DEBUG
/// @macro Debug UDP messages
//#define UDP_DEBUG
/// @macro Debug BestEffortBroadcast
//#define BEB_DEBUG
/// @macro Debug UniformReliableBroadcast
//#define URB_DEBUG
/// @macro Debug UniformReliableBroadcast
//#define LCB_DEBUG
/// @macro Debug Failure Detector
//#define FAILUREDETECTOR_DEBUG
/// @macro Delay after each UDPSender::send
//#define UDPSENDER_DELAY_MS 0.5
```

### References
1. http://edu.epfl.ch/coursebook/en/distributed-algorithms-CS-451
2. Reliable and Secure Distributed Programming. C. Cachin, R. Guerraoui, L. Rodrigues
