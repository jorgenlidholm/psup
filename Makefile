CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld

INCLUDES = -I/usr/arm-linux-gnueabihf/include -I/usr/lib/gcc-cross/arm-linux-gnueabihf/7/include
CFLAGS = -g -Wall
LDFLAGS =

dir_guard=@mkdir -p $(@D)

outputfolder=build_armhf
FILE = fsup
EXE = $(outputfolder)/${FILE}
FOLDERS=$(outputfolder) 

OBJ = ${EXE}.o \
	$(outputfolder)/directory_watcher.o \
	$(outputfolder)/program_watcher.o \
	$(outputfolder)/process_helper.o
CSRC = src/*.c
CHEAD = src/*.h

all: $(EXE) $(CSRC) $(OBJ)

clean:
	rm -rf build *.o *~ core

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@ 

$(outputfolder)/program_watcher.o: src/program_watcher.c
	$(dir_guard)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(outputfolder)/directory_watcher.o: src/directory_watcher.c
	$(dir_guard)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(outputfolder)/process_helper.o: src/process_helper.c src/process_helper.h
	$(dir_guard)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(outputfolder)/fsup.o: src/fsup.c
	$(dir_guard)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

install: all
	@mkdir -p arm
	cp $(EXE) arm/${FILE}
	
push:
	scp build/fsup root@192.168.2.159:~/


