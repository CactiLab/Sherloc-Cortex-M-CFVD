#ifndef BEEBS_H
#define BEEBS_H

// #define CRC32
#define BUBBLESORT
// #define CRC
// #define CUBIC
// #define DIJKSTRA
// #define EDN
// #define FASTA
// #define FIR  // does not work
// #define FRAC
// #define HUFFBENCH  // does not work
// #define LEVENSHTEIN
// #define MATMULT_INT
// #define MERGESORT  // does not work
// #define NBODY
// #define NDES
// #define PICOJPEG
// #define QRDUINO
// #define RIJNDAEL
// #define SG_BINSEARCH
// #define SG_HEAPSORT
// #define SG_QSORT
// #define SG_DLLIST
// #define SG_HASHTABLE
// #define SG_INSERTSORT
// #define SG_LISTSORT
// #define SG_QUEUE
// #define SG_RBTREE
// #define SLRE  // does not work
// #define SQRT
// #define ST
// #define STB_PERLIN
// #define WHETSTONE
// #define WIKISORT
// #define NETTLE_AES
// #define NETTLE_ARCFOUR
// #define NETTLE_SHA256
// #define NETTLE_MD5

#include "support/support.h"

void initialise_benchmark();
int verify_benchmark(int result);
int benchmark();

int BEEBS();

#endif