

/* BEEBS picobenchmark benchmark

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
#include "picojpeg.h"

#include <string.h>

/* This scale factor will be changed to equalise the runtime of the
   benchmarks. */
#define SCALE_FACTOR    (REPEAT_FACTOR >> 0)

const unsigned char jpeg_data[] = {
  0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00, 0x48,
  0x00, 0x48, 0x00, 0x00, 0xff, 0xdb, 0x00, 0x43, 0x00, 0x50, 0x37, 0x3c, 0x46, 0x3c, 0x32, 0x50,
  0x46, 0x41, 0x46, 0x5a, 0x55, 0x50, 0x5f, 0x78, 0xc8, 0x82, 0x78, 0x6e, 0x6e, 0x78, 0xf5, 0xaf,
  0xb9, 0x91, 0xc8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdb, 0x00, 0x43, 0x01, 0x55, 0x5a,
  0x5a, 0x78, 0x69, 0x78, 0xeb, 0x82, 0x82, 0xeb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0,
  0x00, 0x11, 0x08, 0x00, 0x40, 0x00, 0x33, 0x03, 0x01, 0x11, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11,
  0x01, 0xff, 0xc4, 0x00, 0x18, 0x00, 0x00, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x00, 0x04, 0xff, 0xc4, 0x00, 0x26, 0x10,
  0x00, 0x02, 0x02, 0x01, 0x03, 0x03, 0x04, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x02, 0x00, 0x11, 0x03, 0x12, 0x21, 0x31, 0x41, 0x61, 0x71, 0x04, 0x22, 0x53, 0x91, 0x13,
  0x23, 0x51, 0x62, 0xff, 0xc4, 0x00, 0x15, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xc4, 0x00, 0x14, 0x11, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00, 0xc7, 0x88,
  0x0b, 0x28, 0x4d, 0x66, 0xe1, 0x02, 0xcf, 0x58, 0x05, 0x45, 0x88, 0x00, 0xa9, 0x10, 0x34, 0x0e,
  0xaf, 0x6f, 0x68, 0x54, 0x33, 0x64, 0x03, 0x61, 0x02, 0x6a, 0x49, 0xdb, 0x49, 0x84, 0x59, 0x50,
  0x91, 0x50, 0xa0, 0x3d, 0xa4, 0x80, 0x7c, 0x88, 0x18, 0xd5, 0x48, 0x05, 0x09, 0x43, 0xeb, 0xff,
  0x00, 0x2b, 0xf5, 0x02, 0x78, 0xd4, 0x1c, 0xac, 0x48, 0x10, 0x8e, 0xb5, 0x55, 0xa8, 0x0c, 0xa5,
  0x2f, 0xdb, 0x52, 0x2a, 0x19, 0x53, 0xf6, 0x82, 0x04, 0xa1, 0x0e, 0x45, 0xf8, 0x84, 0x0d, 0xad,
  0x7e, 0x21, 0x20, 0xd0, 0x30, 0x04, 0x93, 0x5c, 0x99, 0x43, 0xe2, 0xc6, 0x57, 0x20, 0xd4, 0xd7,
  0x7d, 0x20, 0x3a, 0xe1, 0x40, 0xf6, 0x49, 0xf1, 0x20, 0xa3, 0x8b, 0x17, 0xc5, 0x09, 0x47, 0x26,
  0x35, 0x05, 0xb5, 0x1e, 0x20, 0x75, 0x02, 0x95, 0xc0, 0xfa, 0x81, 0xcf, 0x52, 0x0c, 0x6c, 0x0d,
  0xa0, 0x25, 0xbd, 0xdd, 0xd1, 0xf3, 0x2a, 0x28, 0x43, 0xb0, 0x04, 0x0e, 0x3a, 0xdc, 0x29, 0xb2,
  0x13, 0xf8, 0x6a, 0x11, 0x25, 0x53, 0x22, 0xa9, 0x7d, 0xe0, 0x2e, 0xa8, 0x06, 0xc1, 0xda, 0xa0,
  0x29, 0x21, 0x4d, 0x30, 0x35, 0x2a, 0x28, 0xb9, 0x17, 0x85, 0x06, 0x15, 0x50, 0x97, 0x8c, 0xd8,
  0xef, 0x20, 0xe5, 0x01, 0xf9, 0xd3, 0x52, 0xa1, 0xb5, 0x0e, 0xb0, 0x14, 0x90, 0x39, 0x32, 0x28,
  0xe2, 0x60, 0xd9, 0x17, 0xcc, 0xa2, 0xd9, 0xb1, 0x02, 0x35, 0x58, 0x1e, 0x60, 0x0c, 0x58, 0x95,
  0x06, 0xa6, 0x20, 0xf8, 0x90, 0x3b, 0x67, 0x41, 0xb0, 0xb3, 0x02, 0x4f, 0x9c, 0x55, 0x40, 0x8e,
  0xb5, 0xfe, 0x09, 0x51, 0x12, 0xd7, 0x01, 0xfd, 0x3b, 0x11, 0x99, 0x7b, 0x98, 0x55, 0x7d, 0x4b,
  0xbb, 0x64, 0x00, 0xec, 0xbd, 0x04, 0x05, 0x45, 0x62, 0x36, 0xb2, 0x3b, 0x42, 0x1b, 0x2a, 0x1c,
  0x40, 0x74, 0xb8, 0x11, 0x06, 0x00, 0xb8, 0x1f, 0xff, 0xd9
};
unsigned jpeg_off = 0;

#define MIN(a,b) ((a)<(b)?(a):(b))

unsigned char pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
  unsigned n = MIN(sizeof(jpeg_data) - jpeg_off, buf_size);

  memcpy(pBuf,&jpeg_data[jpeg_off],n);
  *pBytes_actually_read = (unsigned char)n;
  jpeg_off += n;
  return 0;
}



/* This benchmark does not support verification */

int
verify_benchmark_picojpeg (int res __attribute ((unused)) )
{
  return -1;
}


void
initialise_benchmark_picojpeg (void)
{
}


int
benchmark_picojpeg (void)
{
  pjpeg_image_info_t pInfo;
  unsigned char status;

  jpeg_off = 0;

  status = pjpeg_decode_init(&pInfo, pjpeg_need_bytes_callback, 0, 0);

  for(;;)
  {
    status = pjpeg_decode_mcu();

    if(status == PJPG_NO_MORE_BLOCKS)
      break;
  }
  return 0;
}


