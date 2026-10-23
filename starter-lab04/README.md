# Lab 04: Caches and Pipelines

This is a pair programming task, so work on one computer (who's laptop has a
bigger screen) and swap pilot/copilot after each task.

For this lab, we're going to explore how the CPU works on two machines:

- Your sg2000 dev board.
- The laptop you happen to be using.

**Setup**

Install packages on your board: `sudo apt install git build-essential`

Get a Linux-ish build env on your laptop. On Windows, if you don't have a
Linux VM, you can install WSL Debian or something like <https://winlibs.com/>

**Main Task**

Create a file called REPORT.md and answer the numbered questions ("(Qx)") in it.

## Part 1: Characterize Our Caches

Q1. What's the cache layout for the big C906 CPU on the sg2000?

Q2. What's the processor on your laptop? What does hit have for cache layout?

Q3. Run the provided cache.c program on your sg2000. What behavior do you
observe? Is there anything related to your cache layout to explain?

Q4. What does cache.c do? How could you modify it to be maximally interesting to
run on your laptop CPU?

Q5. Run it on your laptop CPU with your modification? Did you get the expected
result? Explain.

Q6. How could you modify cache.c to expose how many ways an L1 cache is?

Q7. Make that modification and run it on the sg2000. What do you observe?

Q8. Get it to actually usefully work to expose wayness. What did you need to do?
What did you observe?

Q9. Run it on your laptop. How many ways does that indicate the L1 cache is?
Does that match your CPUs published specs?

## Part 2: Self-Modifying Code

In ./smc we've got some programs that try to use assembly to directly modify
their own code.

First, try the amd64 Linux or armd64 MacOS version.

Q10. Did it work? Why? If not, can you fix it to work?

Then try the riscv version on the dev board.

Q11. Why didn't it work?

Q12. Can you fix it by sticking nops between the instruction that does the
modification and the modified instruction? How many do you need? Why?

Q13. Is there a simpler solution? See <https://docs.riscv.org/reference/isa/v20260120/unpriv/unpriv-index.html>

Q14. Modern operating systems prevent self modifying code. Is this a good tradeoff? What are we losing?
