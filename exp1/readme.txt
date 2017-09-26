Each binary in this directory was compiled with a value of loop
unrolling corresponding to the binary's filename suffix:

8, 16, 32, 64, 128 , 256, 512, 1024

It is expected that the lower end (8) will be slow due to
frequent malloc-ing for common words, the middle (64-128) will
be faster, and the higher end (256-512) will be slower because
not all of the frequently encountered keywords will have their
linked list node fitting in cache.

