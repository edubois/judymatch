# JUDYMATCH

* Very fast pattern matching proof of concept (microsecond scale using petabytes of data)

This repository contains an implementation of what we call 'Locality Sensitive Hashing'.
Locality Sensitive Hashing functions can provide a very fast way to find neighbors of a given vector of any dimension in a database. It is a lot faster than quadtree, octree, etc.

I invented this in 2008-2009 when I was working on denoising algorithms - it stayed in my head for a while. At this time, I had no idea it was known as 'Locality Senstive Hashing'.

The algorithm provides very nice results with one limitation: sometimes, as I use an approximation as a LSH function, the O(1) matching fails and it takes a little more time to find the actual data.

This project contains a C++ templated wrapper for judy arrays, meaning you can have GREAT results in terms of speed and high scalability.

To me, finding a perfect (non approximative) LSH function is like a big deal in computer science.

To test my current solution that uses DCT as a LSH function:

```
scons mode=release unittest
```

Which gives, on my i7 with USE_JUDY_ARRAY_T defined:

```
Running 2 test cases...
>>>>> Testing find neighborhood of (0, 0) <<<<<<
Generating a database with size: 8388608 vectors of dimension 8 (67108864Bytes)
Trivial result: the closest item is the first one in the judy array...
should_be_closest: [8](0.985446,0.142032,2.08332,0.642395,0.918762,0.400205,0.341242,0.424302)
Distance to correctness: 0
Test PASSED...
>>>>> Testing find neighborhood of a random vector <<<<<
Generating a database with size: 8388608 vectors of dimension 8 (67108864Bytes)
Filling database...
Database filling took: 16370ms.
Find closest element...
Input vector is: [8](3.97585,5.2225,3.35824,2.08398,3.62203,4.76139,4.70915,6.10326)
Finding closest element with LSH algorithm took: 8microsec.
Closest candidate is: [8](2.90514,2.47625,3.41578,2.18761,4.90394,3.12556,3.86751,3.84471)
Now checking the results with a bruteforce algorithm...
Finding closest element with brute force algorithm took: 179ms (179675 microsecs.)
RAW search found: [8](2.90514,2.47625,3.41578,2.18761,4.90394,3.12556,3.86751,3.84471)
Test PASSED...

*** No errors detected
```


## Example of practical applications

* Face matching
* Finger prints matching
* High frequency trading

## Compilation

Please read COMPILATION_TUTORIAL.txt for compilation tutorial.

In default.sconf, you can also enable/disable USE_JUDY_ARRAY_T to enable the use of judy arrays (for amazing performances)
By default, it is disabled.

Note: you must have scons and c++11 to compile.
sudo apt-get install scons g++

