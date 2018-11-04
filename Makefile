MYPROGRAM=da_proc
CC=g++
CXXFLAGS=-O3 -g -std=gnu++11

all: $(MYPROGRAM)

$(MYPROGRAM): main.o \
common.o \
membership.o \
perfectlink.o \
receiver.o \
sender.o \
udpreceiver.o \
udpsender.o \
fifobroadcast.o \
inmemorylog.o \
target.o \
byteprinter.o \
broadcast.o \
besteffortbroadcast.o \
seqtarget.o \
test.o \
uniformreliablebroadcast.o \
threadedreceiver.o \
threadedsender.o \

	$(CC) $^ -o$(MYPROGRAM) -lpthread

%.o : %.cpp *.h
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MYPROGRAM) *.o
