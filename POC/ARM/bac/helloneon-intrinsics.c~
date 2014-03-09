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
void neon_interlace(uint8_t * __restrict d3,uint8_t * __restrict r0,uint8_t * __restrict r1,uint8_t * __restrict r2,int width,int height)
{
	int i;
    for(i=0;i<width*height/8;i++)
    {
  	  uint8_t *ch1 = (uint8_t *)r0[8*i];
  	  uint8_t *ch2 = (uint8_t *)r1[8*i];
  	  uint8_t *ch3 = (uint8_t *)r2[8*i];
  	  uint8_t *s3 = (uint8_t *)d3[8*3*i];

  	  uint8x8x3_t loaded = vld3_u8(s3);
  	  vst1_u8(ch1,loaded.val[0]);
  	  vst1_u8(ch2,loaded.val[1]);
  	  vst1_u8(ch3,loaded.val[2]);

    }

}


