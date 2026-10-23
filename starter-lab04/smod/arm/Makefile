CC=cc
CFLAGS=-g -O1 -Wall

smc: smc.c smc_arm.S
	$(CC) $(CFLAGS) smc.c smc_arm.S -o smc

clean:
	rm -f smc

.PHONY: clean
