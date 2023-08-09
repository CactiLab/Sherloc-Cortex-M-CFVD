/* Common main.c for the benchmarks

   Copyright (C) 2014 Embecosm Limited and University of Bristol
   Copyright (C) 2018 Embecosm Limited

   Contributor: James Pallister <james.pallister@bristol.ac.uk>
   Contributor: Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of the Bristol/Embecosm Embedded Benchmark Suite.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   SPDX-License-Identifier: GPL-3.0-or-later */

// extern void initialise_benchmark(void);
// extern int verify_benchmark(int unused);

#include "BEEBS.h"

#ifndef RTOS
void initialise_benchmark()
{
#if defined(CRC32)
   initialise_benchmark_crc32();
#elif defined(BUBBLESORT)
   initialise_benchmark_bubblesort();
#elif defined(CRC)
   initialise_benchmark_crc();
#elif defined(NETTLE_AES)
   initialise_benchmark_aes();
#elif defined(NETTLE_ARCFOUR)
   initialise_benchmark_arcfour();
#elif defined(NETTLE_SHA256)
   initialise_benchmark_sha256();
#elif defined(NETTLE_MD5)
   initialise_benchmark_md5();
#elif defined(CUBIC)
   initialise_benchmark_cubic();
#elif defined(DIJKSTRA)
   initialise_benchmark_dijkstra();
#elif defined(EDN)
   initialise_benchmark_edn();
#elif defined(FASTA)
   initialise_benchmark_fasta();
#elif defined(FIR)
   initialise_benchmark_fir();
#elif defined(FRAC)
   initialise_benchmark_frac();
#elif defined(HUFFBENCH)
   initialise_benchmark_huffbench();
#elif defined(LEVENSHTEIN)
   initialise_benchmark_levenshtein();
#elif defined(MATMULT_INT)
   initialise_benchmark_matmult_int();
#elif defined(MERGESORT)
   initialise_benchmark_mergesort();
#elif defined(NBODY)
   initialise_benchmark_nbody();
#elif defined(NDES)
   initialise_benchmark_ndes();
#elif defined(PICOJPEG)
   initialise_benchmark_picojpeg();
#elif defined(QRDUINO)
   initialise_benchmark_qrduino();
#elif defined(RIJNDAEL)
   initialise_benchmark_rijndael();
#elif defined(SGLIB_ARRAYBINSEARCH)
   initialise_benchmark_sg_binsearch();
#elif defined(SGLIB_ARRAYHEAPSORT)
   initialise_benchmark_sg_heapsort();
#elif defined(SGLIB_ARRAYQUICKSORT)
   initialise_benchmark_sg_quicksort();
#elif defined(SGLIB_DLLIST)
   initialise_benchmark_sg_dllist();
#elif defined(SGLIB_HASHTABLE)
   initialise_benchmark_sg_hashtable();
#elif defined(SGLIB_LISTINSERTSORT)
   initialise_benchmark_insertsort();
#elif defined(SGLIB_LISTSORT)
   initialise_benchmark_listsort();
#elif defined(SGLIB_QUEUE)
   initialise_benchmark_sg_queue();
#elif defined(SGLIB_RBTREE)
   initialise_benchmark_sg_rbtree();
#elif defined(SLRE)
   initialise_benchmark_slre();
#elif defined(SQRT)
   initialise_benchmark_sqrt();
#elif defined(ST)
   initialise_benchmark_st();
#elif defined(STB_PERLIN)
   initialise_benchmark_stb_perlin();
#elif defined(WHETSTONE)
   initialise_benchmark_whetstone();
#elif defined(WIKISORT)
   initialise_benchmark_wikisort();
#endif
}

int benchmark()
{
#if defined(CRC32)
      return benchmark_crc32();
#elif defined(BUBBLESORT)
      return benchmark_bubblesort();
#elif defined(CRC)
      return benchmark_crc();
#elif defined(NETTLE_AES)
      return benchmark_aes();
#elif defined(NETTLE_ARCFOUR)
      return benchmark_arcfour();
#elif defined(NETTLE_SHA256)
      return benchmark_sha256();
#elif defined(NETTLE_MD5)
      return benchmark_md5();
#elif defined(CUBIC)
      return benchmark_cubic();
#elif defined(DIJKSTRA)
      return benchmark_dijkstra();
#elif defined(EDN)
      return benchmark_edn();
#elif defined(FASTA)
      return benchmark_fasta();
#elif defined(FIR)
      return benchmark_fir();
#elif defined(FRAC)
      return benchmark_frac();
#elif defined(HUFFBENCH)
      return benchmark_huffbench();
#elif defined(LEVENSHTEIN)
      return benchmark_levenshtein();
#elif defined(MATMULT_INT)
      return benchmark_matmult_int();
#elif defined(MERGESORT)
      return benchmark_mergesort();
#elif defined(NBODY)
      return benchmark_nbody();
#elif defined(NDES)
      return benchmark_ndes();
#elif defined(PICOJPEG)
      return benchmark_picojpeg();
#elif defined(QRDUINO)
      return benchmark_qrduino();
#elif defined(RIJNDAEL)
      return benchmark_rijndael();
#elif defined(SGLIB_ARRAYBINSEARCH)
      return benchmark_sg_binsearch();
#elif defined(SGLIB_ARRAYHEAPSORT)
      return benchmark_sg_heapsort();
#elif defined(SGLIB_ARRAYQUICKSORT)
      return benchmark_sg_quicksort();
#elif defined(SGLIB_DLLIST)
      return benchmark_sg_dllist();
#elif defined(SGLIB_HASHTABLE)
      return benchmark_sg_hashtable();
#elif defined(SGLIB_LISTINSERTSORT)
      return benchmark_insertsort();
#elif defined(SGLIB_LISTSORT)
      return benchmark_listsort();
#elif defined(SGLIB_QUEUE)
      return benchmark_sg_queue();
#elif defined(SGLIB_RBTREE)
      return benchmark_sg_rbtree();
#elif defined(SLRE)
      return benchmark_slre();
#elif defined(SQRT)
      return benchmark_sqrt();
#elif defined(ST)
      return benchmark_st();
#elif defined(STB_PERLIN)
      return benchmark_stb_perlin();
#elif defined(WHETSTONE)
      return benchmark_whetstone();
#elif defined(WIKISORT)
      return benchmark_wikisort();
#endif
}

int verify_benchmark(int result)
{
#if defined(CRC32)
   return verify_benchmark_crc32(result);
#elif defined(BUBBLESORT)
   return verify_benchmark_bubblesort(result);
#elif defined(CRC)
   return verify_benchmark_crc(result);
#elif defined(NETTLE_AES)
   return verify_benchmark_aes(result);
#elif defined(NETTLE_ARCFOUR)
   return verify_benchmark_arcfour(result);
#elif defined(NETTLE_SHA256)
   return verify_benchmark_sha256(result);
#elif defined(NETTLE_MD5)
   return verify_benchmark_md5(result);
#elif defined(CUBIC)
   return verify_benchmark_cubic(result);
#elif defined(DIJKSTRA)
   return verify_benchmark_dijkstra(result);
#elif defined(EDN)
   return verify_benchmark_edn(result);
#elif defined(FASTA)
   return verify_benchmark_fasta(result);
#elif defined(FIR)
   return verify_benchmark_fir(result);
#elif defined(FRAC)
   return verify_benchmark_frac(result);
#elif defined(HUFFBENCH)
   return verify_benchmark_huffbench(result);
#elif defined(LEVENSHTEIN)
   return verify_benchmark_levenshtein(result);
#elif defined(MATMULT_INT)
   return verify_benchmark_matmult_int(result);
#elif defined(MERGESORT)
   return verify_benchmark_mergesort(result);
#elif defined(NBODY)
   return verify_benchmark_nbody(result);
#elif defined(NDES)
   return verify_benchmark_ndes(result);
#elif defined(PICOJPEG)
   return verify_benchmark_picojpeg(result);
#elif defined(QRDUINO)
   return verify_benchmark_qrduino(result);
#elif defined(RIJNDAEL)
   return verify_benchmark_rijndael(result);
#elif defined(SGLIB_ARRAYBINSEARCH)
   return verify_benchmark_sg_binsearch(result);
#elif defined(SGLIB_ARRAYHEAPSORT)
   return verify_benchmark_sg_heapsort(result);
#elif defined(SGLIB_ARRAYQUICKSORT)
   return verify_benchmark_sg_quicksort(result);
#elif defined(SGLIB_DLLIST)
   return verify_benchmark_sg_dllist(result);
#elif defined(SGLIB_HASHTABLE)
   return verify_benchmark_sg_hashtable(result);
#elif defined(SGLIB_LISTINSERTSORT)
   return verify_benchmark_insertsort(result);
#elif defined(SGLIB_LISTSORT)
   return verify_benchmark_listsort(result);
#elif defined(SGLIB_QUEUE)
   return verify_benchmark_sg_queue(result);
#elif defined(SGLIB_RBTREE)
   return verify_benchmark_sg_rbtree(result);
#elif defined(SLRE)
   return verify_benchmark_slre(result);
#elif defined(SQRT)
   return verify_benchmark_sqrt(result);
#elif defined(ST)
   return verify_benchmark_st(result);
#elif defined(STB_PERLIN)
   return verify_benchmark_stb_perlin(result);
#elif defined(WHETSTONE)
   return verify_benchmark_whetstone(result);
#elif defined(WIKISORT)
   return verify_benchmark_wikisort(result);
#endif
}
#endif // RTOS

// int BEEBS()
// {
//    int i;
//    volatile int result;
//    int correct;

//    // initialise_board();
//    initialise_benchmark();
//    // start_trigger();

//    for (i = 0; i < REPEAT_FACTOR; i++)
//    {
//       initialise_benchmark();
//       result = benchmark();
//    }

//    // stop_trigger();

//    /* bmarks that use arrays will check a global array rather than int result */
//    correct = verify_benchmark(result);

//    return (!correct);

// } /* main () */