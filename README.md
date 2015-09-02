# JUDYMATCH

* Very fast pattern matching proof of concept (microsecond scale using petabytes of data)

This repository contains an implementation of what we call 'Locality Sensitive Hashing'.
Locality Sensitive Hashing functions can provide a very fast way to find neighbors of a given vector of any dimension in a database. I strongly believe it is a lot faster than quadtree, octree, etc... And have the advantage of beeing dimension-independant.

I actually invented this in 2009 when I was working on denoising algorithms. I had no idea that it was known by the R&D world.
Anyway, the great idea in this project is that I use a judy-array to match vectors.

The whole thing provides very nice results with one limitation: sometimes, as I use an approximation as a LSH function, the matcher gets really wrong. But in most cases, it finds the true best candidate.

This project contains a C++ templated wrapper for judy arrays, meaning you can have GREAT results in terms of speed and high scalability.

To me, finding a perfect (non approximative) LSH function is like a big deal in computer science.

To test my current solution that uses DCT as a LSH function (not very good, I will write why on my blog very soon):

```
scons mode=release unittest
```

## Example of practical applications

* Real time feature tracking
* Face matching
* Finger prints matching
* High frequency trading

## Compilation

Please read COMPILATION_TUTORIAL.txt for compilation tutorial.

In default.sconf, you can also enable/disable USE_JUDY_ARRAY_T to enable the use of judy arrays (for amazing performances)

Note: you must have scons and c++11 to compile.
sudo apt-get install scons g++

