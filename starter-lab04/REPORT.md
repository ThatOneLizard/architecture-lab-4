## Part 1: Characterize Our Caches

### Q1. What's the cache layout for the big C906 CPU on the sg2000?

The cache layout for the C906 CPU is the L1I cache (32kb 2 way) and the L1D 
cache (g4kb 4 way).

### Q2. What's the processor on your laptop? What does hit have for cache layout?

The processor is an apple m4 pro which has an L1 instruction cache (192kb per 
core), an L1 data cache (128kb per core), and an L2 cache (32 MB total shared 
across P-core clusters).

### Q3. Run the provided cache.c program on your sg2000. What behavior do you observe? Is there anything related to your cache layout to explain?

We observed that for each increase in size, the number of bits went up each 
time within roughly the same amount of time throughout most of the tests. The 
size 9 to size 14 also had the exact same time.

sz = 8 (2048 B), time = 0.168s
sz = 9 (4096 B), time = 0.137s
sz = 10 (8192 B), time = 0.137s
sz = 11 (16384 B), time = 0.137s
sz = 12 (32768 B), time = 0.137s
sz = 13 (65536 B), time = 0.137s
sz = 14 (131072 B), time = 0.137s
sz = 15 (262144 B), time = 0.164s
sz = 16 (524288 B), time = 0.204s
sz = 17 (1048576 B), time = 0.197s

### Q4. What does cache.c do? How could you modify it to be maximally interesting to run on your laptop CPU?

cache.c measures how long the computer takes to repeatedly access an array of different sizes.

To make it more interesting, I'd increase the range of memory sizes, say from 1KiB to 256KiB, to see arrays comfortably inside of the L1 cache, all the way up to arrays which will investigate main-memory access.

### Q5. Run it on your laptop CPU with your modification? Did you get the expected result? Explain.

Output:
sz = 7 (1024 B), time = 0.140s
sz = 8 (2048 B), time = 0.164s
sz = 9 (4096 B), time = 0.178s
sz = 10 (8192 B), time = 0.180s
sz = 11 (16384 B), time = 0.179s
sz = 12 (32768 B), time = 0.178s
sz = 13 (65536 B), time = 0.182s
sz = 14 (131072 B), time = 0.182s
sz = 15 (262144 B), time = 0.205s
sz = 16 (524288 B), time = 0.232s
sz = 17 (1048576 B), time = 0.272s
sz = 18 (2097152 B), time = 0.288s
sz = 19 (4194304 B), time = 0.303s
sz = 20 (8388608 B), time = 0.358s
sz = 21 (16777216 B), time = 0.820s
sz = 22 (33554432 B), time = 1.573s
sz = 23 (67108864 B), time = 3.162s
sz = 24 (134217728 B), time = 3.697s
sz = 25 (268435456 B), time = 4.250s

Yes, As the working-set size increased, execution time generally increased because the larger arrays put more pressure on the cache hierarchy and may require more accesses to slower memory. The sharp increase between 8MiB and 16MiB indicates a cache-capacity threshold.

### Q6. How could you modify cache.c to expose how many ways an L1 cache is?

Modify cache.c to access memory locations that map to the same L1 cache set. If we then progressively increase the number of locations accessed, the number of cache misses will increase significantly when we exceed the cache's number of ways.

### Q7. Make that modification and run it on the sg2000. What do you observe?

### Q8. Get it to actually usefully work to expose wayness. What did you need to do? What did you observe?

### Q9. Run it on your laptop. How many ways does that indicate the L1 cache is? Does that match your CPUs published specs?

I observe no change on the 4KiB and 8KiB strides, but on the 16KiB and 32KiB strides, the access time increases from roughly 3ns to 7ns from 8 to 9 locations. This indicates that the cache has 8 ways.

The M4 Pro Mac does infact have 8 ways in the L1 cache.

## Part 2: Self-Modifying Code

In ./smc we've got some programs that try to use assembly to directly modify
their own code.

First, try the amd64 Linux or armd64 MacOS version.

### Q10. Did it work? Why? If not, can you fix it to work?

Then try the riscv version on the dev board.

### Q11. Why didn't it work?

### Q12. Can you fix it by sticking nops between the instruction that does the
modification and the modified instruction? How many do you need? Why?

### Q13. Is there a simpler solution? See <https://docs.riscv.org/reference/isa/v20260120/unpriv/unpriv-index.html>

### Q14. Modern operating systems prevent self modifying code. Is this a good tradeoff? What are we losing?
