# smod -- self-modifying code

A program that rewrites one of its own instructions and then executes it.
The interesting question is whether the CPU's instruction fetch path sees
the new instruction or a stale copy.

Each directory is a C wrapper plus one assembly file:

    smod/amd64   x86-64, Linux / System V
    smod/riscv   RV64, Linux
    smod/arm     arm64, macOS (Mach-O)

Build and run in any of them with:

    make
    ./smc

## The program

    a:  store "jump to c" over the immediate of b
    b:  jump to e            <- patched by a
    c:  return 1             <- success: the patch was seen
    d:  jump to f
    e:  return 0             <- failure: the patch was NOT seen
    f:  ret

The wrapper prints `self modification succeeded` or `self modification
failed` depending on which branch ran.  `a` and `b` are deliberately in the
same 64-byte line: executing `a` has already brought the line holding `b`
into the instruction cache, so if the store never reaches the fetch path,
the stale copy of `b` is what runs.
