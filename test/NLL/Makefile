
# Options for the x86 cross compiler
CC = gcc
CC_OPT = -Wall -I../../include 

# Options for the host C-compiler
CC_HOST = gcc
CC_HOST_OPT = -I.

# Options for the host C++-compiler
CPP_HOST = g++

LIBS = -lpcap

# delete builtin rules
%.o: %.c
#%.o: %.s

OBJS = linux_ver.o ip.o udp.o arp.o eth.o
SRC = linux_ver.c ../../kernel/ip.c ../../kernel/udp.c ../../kernel/arp.c ../../kernel/eth.c

nll: $(OBJS)
	$(CC) -o $(OBJS)  $(LIBS)

$(OBJS): $(SRC)
	$(CC) $(CC_OPT) -DNO_TOS -c -o $@ $<

clean :
	rm -f *~ *.o *.bak *.img

 
