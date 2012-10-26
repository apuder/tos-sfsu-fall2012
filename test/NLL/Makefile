
# Options for the x86 cross compiler
CC = gcc
CC_OPT = -Wall -I../../include 
#-fno-leading-underscore 
#-nostdinc -I../../include 
#-fomit-frame-pointer -fno-defer-pop -fno-leading-underscore -mpreferred-stack-boundary=2 -O -m32 -march=i386 -f$

LD = ld
LD_OPT = -nostdlib -Ttext 0 --oformat elf32-i386 -m elf_i386

# Options for the host C-compiler
CC_HOST = gcc
CC_HOST_OPT = -I.

# Options for the host C++-compiler
CPP_HOST = g++

LD_HOST = gcc
LD_HOST_OPT =

#STRIP=strip

#MAKEDEPEND = makedepend -f-

LIBS = -lpcap

# delete builtin rules
%.o: %.c
#%.o: %.s

OBJS = linux_ver.o ../../kernel/ip.o ../../kernel/udp.o ../../kernel/arp.o ../../kernel/eth.o
SRC = linux_ver.c ../../kernel/ip.c ../../kernel/udp.c ../../kernel/arp.c ../../kernel/eth.c
#DEPS = ../../include/nll.h

#%.o: %.s
#	$(CC) $(CC_OPT) -o $@ -c $<

$(OBJS): $(SRC) #$(DEPS)
	$(CC) $(CC_OPT) -DNO_TOS -c -o $@ $<

nll: $(OBJS)
	$(CC) -o $(OBJS)  $(LIBS)

clean :
	rm -f *~ *.o *.bak *.img

#ifeq (.depend, $(wildcard .depend))
#include .depend
#endif

#.depend:
#	echo '# Module dependencies' > .depend
#	$(MAKEDEPEND) -- $(CC_OPT) $(INCLUDES) -- $(OBJS:.o=.c) >> .depend



#LIBS=-lpcap

#DEPS = ../../include/nll.h ../../include/arp.h ../../include/eth.h ../../include/ip.h ../../include/udp.h

#OBJ = linux_ver.o ../../kernel/ip.o ../../kernel/udp.o ../../kernel/arp.o ../../kernel/eth.o 


#%.o: %.c $(DEPS)
#$(CC) $(CC_OPT) $(DEPS) -c -o $@ $< 

#nll: $(OBJ)
#$(CC) -o $@ $^ $(LIBS)


#clean:
#rm -f *.o *~ 
