MYPROGRAM=da_proc
CC=g++
CXXFLAGS=-O0 -g

all: $(MYPROGRAM)

$(MYPROGRAM): main.o common.o membership.o perfectreceiver.o perfectsender.o receiver.o sender.o udpreceiver.o udpsender.o
	$(CC) $^ -o$(MYPROGRAM)

%.o : %.cpp %.h
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MYPROGRAM) *.o
