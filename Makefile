MYPROGRAM=da_proc
CC=g++
CXXFLAGS=-O0 -g

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

	$(CC) $^ -o$(MYPROGRAM) -lpthread

%.o : %.cpp %.h
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MYPROGRAM) *.o
