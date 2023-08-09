/* Copyright (C) 2014 Embecosm Limited and the University of Bristol

   Contributor James Pallister <james.pallister@bristol.ac.uk>

   This file is part of BEEBS

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along with
   this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SUPPORT_H
#define SUPPORT_H

/* Board repeat factor may be set in the board support header, if not, we
   define a default */

#ifndef BOARD_REPEAT_FACTOR
// #define BOARD_REPEAT_FACTOR 4096
#define BOARD_REPEAT_FACTOR 256

#endif

/* Scaling factors may defined when compiling benchmarks. If not we set it to
   zero, which means no scaling and then leads to the REPEAT_FACTOR for the
   program. */

#ifndef CALIB_SCALE
#define CALIB_SCALE 0
#endif

#if defined(CRC32)
void initialise_benchmark_crc32();
int verify_benchmark_crc32(int result);
int benchmark_crc32(void) __attribute__((noinline));
#endif
#if defined(BUBBLESORT)
void initialise_benchmark_bubblesort();
int verify_benchmark_bubblesort(int result);
int benchmark_bubblesort(void) __attribute__((noinline));
#endif
#if defined(CUBIC)
void initialise_benchmark_cubic();
int verify_benchmark_cubic(int result);
int benchmark_cubic(void) __attribute__((noinline));
#endif
#if defined(DIJKSTRA)
void initialise_benchmark_dijkstra();
int verify_benchmark_dijkstra(int result);
int benchmark_dijkstra(void) __attribute__((noinline));
#endif
#if defined(EDN)
void initialise_benchmark_edn();
int verify_benchmark_edn(int result);
int benchmark_edn(void) __attribute__((noinline));
#endif
#if defined(FASTA)
void initialise_benchmark_fasta();
int verify_benchmark_fasta(int result);
int benchmark_fasta(void) __attribute__((noinline));
#endif
#if defined(FIR)
void initialise_benchmark_fir();
int verify_benchmark_fir(int result);
int benchmark_fir(void) __attribute__((noinline));
#endif
#if defined(FRAC)
void initialise_benchmark_frac();
int verify_benchmark_frac(int result);
int benchmark_frac(void) __attribute__((noinline));
#endif
#if defined(HUFFBENCH)
void initialise_benchmark_huffbench();
int verify_benchmark_huffbench(int result);
int benchmark_huffbench(void) __attribute__((noinline));
#endif
#if defined(LEVENSHTEIN)
void initialise_benchmark_levenshtein();
int verify_benchmark_levenshtein(int result);
int benchmark_levenshtein(void) __attribute__((noinline));
#endif
#if defined(MATMULT_INT)
void initialise_benchmark_matmult_int();
int verify_benchmark_matmult_int(int result);
int benchmark_matmult_int(void) __attribute__((noinline));
#endif
#if defined(MERGESORT)
void initialise_benchmark_mergesort();
int verify_benchmark_mergesort(int result);
int benchmark_mergesort(void) __attribute__((noinline));
#endif
#if defined(NBODY)
void initialise_benchmark_nbody();
int verify_benchmark_nbody(int result);
int benchmark_nbody(void) __attribute__((noinline));
#endif
#if defined(NDES)
void initialise_benchmark_ndes();
int verify_benchmark_ndes(int result);
int benchmark_ndes(void) __attribute__((noinline));
#endif
#if defined(NETTLE_AES)
void initialise_benchmark_aes();
int verify_benchmark_aes(int result);
int benchmark_aes(void) __attribute__((noinline));
#endif
#if defined(PICOJPEG)
void initialise_benchmark_picojpeg();
int verify_benchmark_picojpeg(int result);
int benchmark_picojpeg(void) __attribute__((noinline));
#endif
#if defined(QRDUINO)
void initialise_benchmark_qrduino();
int verify_benchmark_qrduino(int result);
int benchmark_qrduino(void) __attribute__((noinline));
#endif
#if defined(RIJNDAEL)
void initialise_benchmark_rijndael();
int verify_benchmark_rijndael(int result);
int benchmark_rijndael(void) __attribute__((noinline));
#endif
#if defined(SGLIB_ARRAYBINSEARCH)
void initialise_benchmark_sg_binsearch();
int verify_benchmark_sg_binsearch(int result);
int benchmark_sg_binsearch(void) __attribute__((noinline));
#endif
#if defined(SGLIB_HEAPSORT)
void initialise_benchmark_sg_heapsort();
int verify_benchmark_sg_heapsort(int result);
int benchmark_sg_heapsort(void) __attribute__((noinline));
#endif
#if defined(SGLIB_QSORT)
void initialise_benchmark_sg_quicksort();
int verify_benchmark_sg_quicksort(int result);
int benchmark_sg_quicksort(void) __attribute__((noinline));
#endif
#if defined(SGLIB_DLLIST)
void initialise_benchmark_sg_dllist();
int verify_benchmark_sg_dllist(int result);
int benchmark_sg_dllist(void) __attribute__((noinline));
#endif
#if defined(SGLIB_HASHTABLE)
void initialise_benchmark_sg_hashtable();
int verify_benchmark_sg_hashtable(int result);
int benchmark_sg_hashtable(void) __attribute__((noinline));
#endif
#if defined(SGLIB_INSERTSORT)
void initialise_benchmark_insertsort();
int verify_benchmark_insertsort(int result);
int benchmark_insertsort(void) __attribute__((noinline));
#endif
#if defined(SGLIB_LISTSORT)
void initialise_benchmark_listsort();
int verify_benchmark_listsort(int result);
int benchmark_listsort(void) __attribute__((noinline));
#endif
#if defined(SGLIB_QUEUE)
void initialise_benchmark_sg_queue();
int verify_benchmark_sg_queue(int result);
int benchmark_sg_queue(void) __attribute__((noinline));
#endif
#if defined(SGLIB_RBTREE)
void initialise_benchmark_sg_rbtree();
int verify_benchmark_sg_rbtree(int result);
int benchmark_sg_rbtree(void) __attribute__((noinline));
#endif
#if defined(SLRE)
void initialise_benchmark_slre();
int verify_benchmark_slre(int result);
int benchmark_slre(void) __attribute__((noinline));
#endif
#if defined(SQRT)
void initialise_benchmark_sqrt();
int verify_benchmark_sqrt(int result);
int benchmark_sqrt(void) __attribute__((noinline));
#endif
#if defined(ST)
void initialise_benchmark_st();
int verify_benchmark_st(int result);
int benchmark_st(void) __attribute__((noinline));
#endif
#if defined(STB_PERLIN)
void initialise_benchmark_stb_perlin();
int verify_benchmark_stb_perlin(int result);
int benchmark_stb_perlin(void) __attribute__((noinline));
#endif
#if defined(WHETSTONE)
void initialise_benchmark_whetstone();
int verify_benchmark_whetstone(int result);
int benchmark_whetstone(void) __attribute__((noinline));
#endif
#if defined(WIKISORT)
void initialise_benchmark_wikisort();
int verify_benchmark_wikisort(int result);
int benchmark_wikisort(void) __attribute__((noinline));
#endif

// T  overall repeat factor is scaled by the command-line given
// CALIB_SCALE.

#define REPEAT_FACTOR (((CALIB_SCALE) > 0)                          \
                           ? (BOARD_REPEAT_FACTOR) >> (CALIB_SCALE) \
                           : (BOARD_REPEAT_FACTOR) << (-CALIB_SCALE))

// void initialise_benchmark(void);
/* Benchmarks must implement verify_benchmark, which must return -1 if no
   verification is done. */

// int verify_benchmark(int result);

/* Standard functions implemented for each board */

void initialise_board(void);
void start_trigger(void);
void stop_trigger(void);

/* Every benchmark implements this as its entry point. Don't allow it to be
   inlined! */

// int benchmark(void) __attribute__((noinline));

#endif /* SUPPORT_H */

/*
   Local Variables:
   mode: C++
   c-file-style: "gnu"
   End:
*/
