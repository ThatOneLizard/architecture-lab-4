CC=gcc
CFLAGS=-g -O1 -Wall

smc: smc.c smc_riscv.S
	$(CC) $(CFLAGS) smc.c smc_riscv.S -o smc

clean:
	rm -f smc

.PHONY: clean
