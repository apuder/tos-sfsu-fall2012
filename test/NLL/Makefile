
# Options for the x86 cross compiler
CC = gcc
CC_OPT = -Wall -fno-stack-protector 
CFLAGS = -I../../include

LD = ld
LD_OPT = -m elf_i386
#--oformat elf32-i386 
#-m elf_i386

# pcap standard library 

LIB = -lpcap
OBJ_DIR = objects
SRC_DIR = ../../kernel

_OBJ = linux_ver.o arp.o ip.o eth.o udp.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

_SRC = arp.c ip.c eth.c udp.c
SRC  = $(patsubst %,$(SRC_DIR)/%,$(_SRC))



$(OBJ_DIR)/%.o:%.c
	$(CC) $(CC_OPT) -DNO_TOS -c -o $@ $< $(CFLAGS)

nll: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

clean :
	rm -f *~ *.o $(OBJ_DIR)/*.o *.bak *.img

