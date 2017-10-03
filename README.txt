CSC 252 Project 4 Cache Simulator README
Po-Chun Chiu & Yujie Liu
Email: pchiu4@u.rochester.edu, yliu134@u.rochester.edu

COMPILE STEPS:
Compile the program using makefile and and run the program using commands under Linux or Mac Terminal

Sample Compile Steps:
cd csc252-project4
make
./cacheSim -t traces/gcc.trace -s 16 -w 1 -l 32
./cacheSim -t traces/gzip.trace -s 16 -w 1 -l 32
./cacheSim -t traces/swim.trace -s 32 -w 2 -l 64
./cacheSim -t traces/mcf.trace -s 64 -w 4 -l 64
./cacheSim -t traces/twolf.trace -s 32 -w 2 -l 64
(The results are tested using the CSUG cycle machine!)
Discussion:
We didn't modify anything in the trace.c and trace.h. We add the functions we use in cache.c and cache.h. We mainly do the work in cache.c
We use the FIFO replace policy for replacement and write-back modifications.  We have our simulated files named after .trace file. For example, we have gcc.trace, so our result files would be named gcc.trace.simulated. Same for all the results.
And all of our trace.simulated files are put inside the traces file.
The TAs can also use the diff to test the results:
Example is: diff GOLD/mcf_s64_w4_l64.GOLD traces/mcf.trace.simulated 
