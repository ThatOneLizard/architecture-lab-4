CC=gcc
CFLAGS=-g -O1 -Wall

smc: smc.c smc_amd64.S
	$(CC) $(CFLAGS) smc.c smc_amd64.S -o smc

clean:
	rm -f smc

.PHONY: clean
