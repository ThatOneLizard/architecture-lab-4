/*  smc_arm.S -- a self-modifying jump, arm64 / macOS (Mach-O)
 *
 *  Same six instructions as smod/amd64 and smod/riscv:
 *
 *    a:  store "b c" over the immediate of b
 *    b:  b e              <- patched by a to become "b c"
 *    c:  w0 = 1           <- taken if the patch was seen:     success
 *    d:  b f
 *    e:  w0 = 0           <- taken if the patch was NOT seen: failure
 *    f:  ret
 *
 *  a and b are in the same 64-byte I-cache line on purpose: executing a has
 *  already pulled in the line holding b, so if the store in a never reaches
 *  the instruction fetch path, the CPU executes a stale copy of b and we take
 *  the failure branch.
 *
 *  arm64 does not keep the instruction cache coherent with the data cache.
 *  The store lands in the D-cache; the fetch of b reads the I-cache, which
 *  still holds the old instruction.  The fix is the cache maintenance
 *  sequence noted at the bottom.
 *
 *  NOTE: on Apple Silicon the store in a faults before any of this matters,
 *  because the page cannot be made writable (see smc.c).  This build is for
 *  arm64 Linux, or for Intel macOS.
 */

	.text
	.p2align 6

	.globl	_smc_entry
_smc_entry:
a:
	adr	x0, b			/* address of the instruction we patch */
	adr	x2, c
	sub	x2, x2, x0		/* imm = c - b, in bytes */
	lsr	x2, x2, #2		/* imm26, in instructions */
	and	x2, x2, #0x3ffffff
	movz	w3, #0x1400, lsl #16	/* 0x14000000: opcode for b */
	orr	w2, w2, w3
	str	w2, [x0]
b:
	b	e
c:
	mov	w0, #1
d:
	b	f
e:
	mov	w0, #0
f:
	ret

/*  The fix, once you have worked out what is missing:
 *
 *      adr x0, b               # or the range you modified
 *      dc  cvau, x0            # clean D-cache to the point of unification
 *      dsb ish
 *      ic  ivau, x0            # invalidate I-cache to the same point
 *      dsb ish
 *      isb                     # flush the front end of the pipeline
 *
 *  The isb is the pipeline half: without it the I-cache can be clean and the
 *  front end can still have the old b already decoded.
 */
