judymatch
=========

Very fast pattern matching algorithm

The final goal of this library is gives a set of tools to help people in finding and testing their Locality Sensitive Hashing functions.

It contains a C++ templated wrapper for judy arrays, meaning you can have AMAZING results in term of speed and high scalability.

Locality Sensitive Hashing functions can provide a very fast way to find neighbors of a given vector of any dimension in a database.
I strongly believe it is a lot faster thank quadtree, octree, etc...

To me, finding a perfect (non approximative) LSH function is like a "Saint Graal" of computer science.
I am working on a perfect LSH function.

https://github.com/edubois/judymatch

Don't forget to pull submodules:
git submodule update -i

To test my current solution that uses DCT as a LSH function (not very good, I will write on my blog why very soon):
Edit you default.sconf, fill with the right libraries and path, then:

scons mode=release unittest

Note: you must have scons to compile.
sudo apt-get install scons
