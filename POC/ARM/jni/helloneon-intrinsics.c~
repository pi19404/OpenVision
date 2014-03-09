/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "helloneon-intrinsics.h"


/* this source file should only be compiled by Android.mk when targeting
 * the armeabi-v7a ABI, and should be built in NEON mode
 */
typedef uchar (unsigned char);
void neon_deinterlace(uint8_t * __restrict d3,uint8_t * __restrict r0,uint8_t * __restrict r1,uint8_t * __restrict r2,int width,int height)
{
	int i;
	uint8x8x3_t loaded;
    for(i=0;i<width*height/8;i++)
    {
  	  loaded = vld3_u8(d3);
  	  vst1_u8(r0,loaded.val[0]);
  	  vst1_u8(r1,loaded.val[1]);
  	  vst1_u8(r2,loaded.val[2]);
  	  d3=d3+3*8;
  	  r0=r0+8;
  	  r1=r1+8;
  	  r2=r2+8;
    }

}



void neon_interlace(uint8_t * __restrict d3,uint8_t * __restrict r0,uint8_t * __restrict r1,uint8_t * __restrict r2,int width,int height)
{
	int i;
	uint8x8x3_t v;
    for(i=0;i<width*height/8;i++)
    {
  	  v.val[0]=vld1_u8(r0);
  	  v.val[1]=vld1_u8(r1);
  	  v.val[2]=vld1_u8(r2);
  	  vst3_u8(d3,v);
  	  d3=d3+3*8;
  	  r0=r0+8;
  	  r1=r1+8;
  	  r2=r2+8;
    }

}


