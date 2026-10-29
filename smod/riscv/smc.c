/*  smc.c -- C wrapper for the self-modifying jump.
 *
 *  The wrapper does one thing and then gets out of the way: it makes the
 *  text page writable so that instruction a can store into instruction b.
 *
 *  W^X
 *  ---
 *  The text segment is mapped read-only, and on most systems the kernel is
 *  allowed to insist on "write XOR execute": a page may be writable or
 *  executable, but not both at once.  Left alone, the store in a faults
 *  before we ever reach the interesting part -- delete the mprotect() call
 *  below and you get a SIGSEGV, not a wrong answer.  That is a different
 *  lesson from this one, so we pay the bribe up front and then study what
 *  the instruction fetch path does with the modified code.
 *
 *  On Linux one mprotect() with PROT_WRITE|PROT_EXEC is enough.  On
 *  macOS/Apple Silicon it is not; see smod/arm/smc.c.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

extern int smc_entry(void);

static int
make_writable(void *addr, size_t len)
{
	long ps = sysconf(_SC_PAGESIZE);
	uintptr_t lo = (uintptr_t)addr & ~(uintptr_t)(ps - 1);
	uintptr_t hi = ((uintptr_t)addr + len + (uintptr_t)ps - 1) & ~(uintptr_t)(ps - 1);

	return mprotect((void *)lo, (size_t)(hi - lo),
			PROT_READ | PROT_WRITE | PROT_EXEC);
}

int
main(int argc, char *argv[])
{
	if (make_writable((void *)smc_entry, 64) != 0) {
		perror("mprotect");
		return 1;
	}

	int r = smc_entry();

	printf("self modification %s\n", r ? "succeeded" : "failed");
	return 0;
}
