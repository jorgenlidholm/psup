CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld

INCLUDES = -I/usr/arm-linux-gnueabihf/include -I/usr/lib/gcc-cross/arm-linux-gnueabihf/7/include
CFLAGS = -g -Wall
LDFLAGS =

EXE = build/fsup

OBJ = ${EXE}.o
CSRC = src/*.c
CHEAD = src/*.h

all: $(EXE) $(CSRC) $(OBJ)

clean:
	rm -rf build/* *.o *~ core

$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@ 

$(OBJ): $(CSRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(CSRC) -o $@

push:
	scp build/fsup root@192.168.2.159:~/


