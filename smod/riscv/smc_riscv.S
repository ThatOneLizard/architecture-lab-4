/*  smc_riscv.S -- a self-modifying jump, RV64 / Linux
 *
 *    a:  encode "jal x0, c" and store it over b
 *    b:  j e              <- patched by a to become "j c"
 *    c:  a0 = 1           <- taken if the patch was seen:     success
 *    d:  j f
 *    e:  a0 = 0           <- taken if the patch was NOT seen: failure
 *    f:  ret
 *
 *  a and b sit in the same I-cache line on purpose: executing a has already
 *  pulled in the line holding b, so if the store in a never reaches the
 *  instruction fetch path, the CPU executes a stale copy of b and we take the
 *  failure branch.
 *
 *  RISC-V does not keep the instruction cache coherent with the data cache.
 *  The store lands in the D-cache; the fetch of b reads the I-cache, which
 *  still holds the old instruction.  The architecturally required fix is
 *  fence.i between the store and the branch (see the note at the bottom).
 *
 *  The immediate encoding below is the J-type, which is deliberately awkward:
 *
 *      bits 31 | 30:21 | 20 | 19:12 | 11:7 | 6:0
 *           [20]  [10:1]  [11]  [19:12]  rd   opcode
 */

	.option	 norvc			/* every instruction 4 bytes wide */
	.option	 norelax		/* and stay that wide after linking:
					   the linker will otherwise happily
					   compress "j e" to the 2-byte c.j and
					   our 4-byte store would corrupt c */
	.text
	.p2align 6

	.globl	smc_entry
	.type	smc_entry, @function
smc_entry:
a:
	la	t0, b			/* address of the instruction we patch */
	la	t1, c
	sub	t1, t1, t0		/* imm = c - b, in bytes */

	srli	t2, t1, 20		/* imm[20]  -> bit 31 */
	andi	t2, t2, 1
	slli	t2, t2, 31

	srli	t3, t1, 1		/* imm[10:1] -> bits 30:21 */
	andi	t3, t3, 0x3ff
	slli	t3, t3, 21
	or	t2, t2, t3

	srli	t3, t1, 11		/* imm[11]  -> bit 20 */
	andi	t3, t3, 1
	slli	t3, t3, 20
	or	t2, t2, t3

	srli	t3, t1, 12		/* imm[19:12] -> bits 19:12 */
	andi	t3, t3, 0xff
	slli	t3, t3, 12
	or	t2, t2, t3

	ori	t2, t2, 0x6f		/* rd = x0, opcode = JAL */
	sw	t2, 0(t0)
	fence.i
b:
	j	e
c:
	li	a0, 1
d:
	j	f
e:
	li	a0, 0
f:
	ret
	.size	smc_entry, . - smc_entry

	.section .note.GNU-stack,"",@progbits

