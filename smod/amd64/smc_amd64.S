/*  smc_amd64.S -- a self-modifying jump, x86-64 / System V / Linux
 *
 *  Six instructions, laid out the way the lab describes:
 *
 *    a:  store the displacement for "jmp c" into the immediate of b
 *    b:  jmp e            <- patched by a to become "jmp c"
 *    c:  eax = 1          <- taken if the patch was seen:     success
 *    d:  jmp f
 *    e:  eax = 0          <- taken if the patch was NOT seen: failure
 *    f:  ret
 *
 *  a and b are in the same 64-byte I-cache line on purpose: executing a has
 *  already pulled in the line holding b, so if the store in a never reaches
 *  the instruction fetch path, the CPU executes a stale copy of b and we take
 *  the failure branch.  That is the whole experiment.
 *
 *  x86-64 gets this right in hardware.  The store snoops the instruction
 *  cache and the pipeline, so the patched b is what executes.  Architecturally
 *  the SDM only asks for a serialising instruction after the store; in
 *  practice you never need it.
 */

	.text
	.p2align 6			/* a and b in one I-cache line */

	.globl	smc_entry
	.type	smc_entry, @function
smc_entry:
a:
	movl	$(c - b - 5), b+1(%rip)	/* overwrite b's rel32 */
b:
	.byte	0xe9			/* force "jmp rel32" (5 bytes, not 2) */
	.long	e - b - 5		/* initially: jmp e */
c:
	movl	$1, %eax
d:
	jmp	f
e:
	xorl	%eax, %eax
f:
	ret
	.size	smc_entry, . - smc_entry

	.section .note.GNU-stack,"",@progbits
