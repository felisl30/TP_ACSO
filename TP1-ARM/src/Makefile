# sim: shell.c sim.c 
# 	gcc -g -O0 $^ -o $@

# .PHONY: clean
# clean:
# 	rm -rf *.o *~ sim

CC = gcc
CFLAGS = -Wall -g

all: sim

sim: sim.o instruction_handlers.o shell.o
	$(CC) $(CFLAGS) -o sim sim.o instruction_handlers.o shell.o

sim.o: sim.c instruction_handlers.h shell.h
	$(CC) $(CFLAGS) -c sim.c

instruction_handlers.o: instruction_handlers.c instruction_handlers.h shell.h
	$(CC) $(CFLAGS) -c instruction_handlers.c

shell.o: shell.c shell.h
	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f sim *.o