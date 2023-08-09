/* BEEBS listsort benchmark

   Copyright (C) 2014 Embecosm Limited and University of Bristol

   Contributor James Pallister <james.pallister@bristol.ac.uk>

   This file is part of the Bristol/Embecosm Embedded Benchmark Suite.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include "../../support/support.h"
#include <stdio.h>
#include <string.h>
#include "sglib.h"
#include <stdlib.h>

/* This scale factor will be changed to equalise the runtime of the
   benchmarks. */
#define SCALE_FACTOR    (REPEAT_FACTOR >> 0)


struct ilist {
    int i;
    struct ilist *next_ptr;
};

#define ILIST_COMPARATOR(e1, e2) (e1->i - e2->i)

int array[100] = {14, 66, 12, 41, 86, 69, 19, 77, 68, 38, 26, 42, 37, 23, 17, 29, 55, 13,
  90, 92, 76, 99, 10, 54, 57, 83, 40, 44, 75, 33, 24, 28, 80, 18, 78, 32, 93, 89, 52, 11,
  21, 96, 50, 15, 48, 63, 87, 20, 8, 85, 43, 16, 94, 88, 53, 84, 74, 91, 67, 36, 95, 61,
  64, 5, 30, 82, 72, 46, 59, 9, 7, 3, 39, 31, 4, 73, 70, 60, 58, 81, 56, 51, 45, 1, 6, 49,
  27, 47, 34, 35, 62, 97, 2, 79, 98, 25, 22, 65, 71, 0};

/* BEEBS heap is just an array */

#include <stddef.h>

#define HEAP_SIZE 8192
static char heap[HEAP_SIZE];
static void *heap_ptr;
static void *heap_end;

/* Initialize the BEEBS heap pointers */

static void
init_heap (void)
{
    heap_ptr = (void *) heap;
    heap_end = heap_ptr + HEAP_SIZE;
}

/* BEEBS version of malloc.

   This is primarily to reduce library and OS dependencies. Malloc is
   generally not used in embedded code, or if it is, only in well defined
   contexts to pre-allocate a fixed amount of memory. So this simplistic
   implementation is just fine. */

static void *
malloc_beebs (size_t size)
{
    void *new_ptr = heap_ptr;

    if (((heap_ptr + size) > heap_end) || (0 == size))
	return NULL;
    else
	{
	    heap_ptr += size;
	    return new_ptr;
	}
}

/* BEEBS version of free.

   For our simplified version of memory handling, free can just do nothing. */

static void
free_beebs (void *ptr)
{
}

void
initialise_benchmark_listsort (void)
{
  init_heap ();
}



int benchmark_listsort()
{
  int i;
  struct ilist *l, *the_list;
  int cnt = 0;

  the_list = NULL;
  for (i=0; i<100; i++) {
    l = malloc_beebs(sizeof(struct ilist));
    l->i = array[i];
    SGLIB_LIST_ADD(struct ilist, the_list, l, next_ptr);
  }
  // it is useless, but anyway, get parameters in the right order
  SGLIB_LIST_REVERSE(struct ilist, the_list, next_ptr);
  // now sort them
  SGLIB_LIST_SORT(struct ilist, the_list, ILIST_COMPARATOR, next_ptr);
  // print the list
  SGLIB_LIST_MAP_ON_ELEMENTS(struct ilist, the_list, ll, next_ptr, {
    cnt += ll->i;
  });
  // free all
  SGLIB_LIST_MAP_ON_ELEMENTS(struct ilist, the_list, ll, next_ptr, {
    free_beebs(ll);
  });
  return cnt;
}

int verify_benchmark_listsort(int r) {
  int expected = 4950;
  if (r != expected)
    return 0;
  return 1;
}
