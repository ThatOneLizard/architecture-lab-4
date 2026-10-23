/*  smc.c -- C wrapper for the self-modifying jump, arm64 / macOS.
 *
 *  Same job as the Linux wrapper: make the text page writable so that
 *  instruction a can store into instruction b.
 *
 *  W^X on Apple Silicon
 *  --------------------
 *  This call is refused on arm64 macOS.  A page cannot be both writable and
 *  executable there; mprotect() with PROT_WRITE|PROT_EXEC comes back EACCES
 *  unless the mapping was created with MAP_JIT, and even then the kernel
 *  flips the permission per thread rather than handing out a page you can
 *  write and execute at once.  So the page stays executable-but-not-writable
 *  and the store in a faults.
 *
 *  That is left alone here on purpose: getting past it needs MAP_JIT and a
 *  copy of the code into a JIT page, which is a different experiment.  The
 *  interesting part of this lab -- whether the patched instruction reaches
 *  the instruction fetch path -- is the same on any arm64 Linux machine.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

extern int smc_entry(void);

int
main(int argc, char *argv[])
{
	long ps = sysconf(_SC_PAGESIZE);
	uintptr_t lo = (uintptr_t)smc_entry & ~(uintptr_t)(ps - 1);

	/* Fails on Apple Silicon; the store in a faults afterwards. */
	if (mprotect((void *)lo, (size_t)ps,
		     PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
		perror("mprotect (page left executable but not writable)");

	int r = smc_entry();

	printf("self modification %s\n", r ? "succeeded" : "failed");
	return 0;
}
