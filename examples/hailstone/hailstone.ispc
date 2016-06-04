/*
  Copyright (c) 2010-2012, Intel Corporation
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.


   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/



export void hailstone_bad(uniform int values[], uniform int length) {
    for(int i = programIndex; i < length; i+=programCount) {
        int iters = 0;
        int value = i+1;
        while(value != 1) {
            iters++;
            if((value % 2) == 0) //even
                value /= 2;//value /= 2;
            else //odd
                value = value *3 + 1;
        }
        values[i] = iters;
    }
}

/*
export void hailstone_ispc(uniform int values[], uniform int length) {
    for(int i = programIndex; i < length; i+=programCount) {
        int iters = 0;
        int value = i+1;
        while(value != 1) {
            iters++;
            unmasked {
            if((value &1) == 0) //even
                value = value >> 1;//value /= 2;
            else //odd
                value = value *3 + 1;
        }}
        values[i] = iters;
    }
}
*/


export void hailstone_ispc(uniform int values[], uniform int length) {
    int i = programIndex;
    int iters = 0;
    int value = i+1;
    uniform int top = programCount;
    while(true) {
        cif(value != 1) {
            iters++;
            unmasked{
                if((value&1) == 0)
                    value = value >> 1;
                else
                    value = 3*value +1;
            }
        }else{
            values[i] = iters;
            foreach_active(index)
                i = top++;
            iters = 0;
            value = i+1;
            if(i > length) break;
        }
    }
}



/*
[bad ispc]:		[13.608] million cycles
[hailstone ispc]:		[10.552] million cycles // modulo -> and
[hailstone serial]:		[12.411] million cycles
				(1.18x speedup from ISPC)


[bad ispc]:		[13.652] million cycles
[hailstone ispc]:		[10.140] million cycles // complete strength reduction
[hailstone serial]:		[12.589] million cycles
				(1.24x speedup from ISPC)

[bad ispc]:		[13.543] million cycles
[hailstone ispc]:		[9.398] million cycles // strength reduction + unmasked
[hailstone serial]:		[12.580] million cycles
				(1.34x speedup from ISPC)

[bad ispc]:		[13.562] million cycles
[hailstone ispc]:		[7.235] million cycles // rewrite as one loop
[hailstone serial]:		[12.588] million cycles
				(1.74x speedup from ISPC)


*/
