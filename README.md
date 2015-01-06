judymatch
=========

Very fast pattern matching framework

The final goal of this library is to give a set of tools to help people in finding and testing their Locality Sensitive Hashing functions.

It contains a C++ templated wrapper for judy arrays, meaning you can have AMAZING results in terms of speed and high scalability. This feature has been disabled until I find a perfect hashing function.

Locality Sensitive Hashing functions can provide a very fast way to find neighbors of a given vector of any dimension in a database. I strongly believe it is a lot faster than quadtree, octree, etc... And have the advantage of beeing dimension-independant.

To me, finding a perfect (non approximative) LSH function is like a big deal in computer science.
I am working on a perfect LSH function.

Please read COMPILATION_TUTO.txt for compilation tutorial.

To test my current solution that uses DCT as a LSH function (not very good, I will write why on my blog very soon):

scons mode=release unittest

In default.sconf, you can also enable USE_JUDY_ARRAY_T to enable the use of judy arrays (for amazing performances)

Note: you must have scons to compile.
sudo apt-get install scons
