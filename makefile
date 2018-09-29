SOURCE=main.cpp common.cpp membership.cpp perfectreceiver.cpp perfectsender.cpp receiver.cpp sender.cpp udpreceiver.cpp udpsender.cpp
MYPROGRAM=da_proc
CC=g++

all: $(MYPROGRAM)

$(MYPROGRAM): $(SOURCE)
	$(CC) $(SOURCE) -o$(MYPROGRAM)

clean:
	rm -f $(MYPROGRAM)
