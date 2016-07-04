/*
  Copyright (c) 2010-2011, Intel Corporation
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#pragma warning (disable: 4244)
#pragma warning (disable: 4305)
#endif

#include <stdio.h>
#include <algorithm>
#include "../timing.h"
#include "hailstone_ispc.h"
#include <string.h>
#include <cstdlib>
using namespace ispc;

extern void hailstone_serial(int values[], int length);


int main(int argc, char *argv[]) {
    static unsigned int test_iterations[] = {30, 30};
    int *values = new int[20900];
    int length = 20900;


    double minISPC = 1e30;
    for (unsigned int i = 0; i < test_iterations[0]; ++i) {
        reset_and_start_timer();
        hailstone_ispc(values, length);
        double dt = get_elapsed_mcycles();
        //printf("@time of ISPC run:\t\t\t[%.3f] million cycles\n", dt);
        minISPC = std::min(minISPC, dt);
    }

    printf("[original ispc]:\t\t[%.3f] million cycles\n", minISPC);
    
    // Clear out the buffer
    for (unsigned int i = 0; i < length; ++i)
        values[i] = 0;
        
    //
    // Compute the hailstone sequence using the ispc implementation; report the minimum
    // time of three runs.
    //
    //double minISPC = 1e30;
    for (unsigned int i = 0; i < test_iterations[0]; ++i) {
        reset_and_start_timer();
        hailstone_ispc_strengthreduct(values, length);
        double dt = get_elapsed_mcycles();
        //printf("@time of ISPC run:\t\t\t[%.3f] million cycles\n", dt);
        minISPC = std::min(minISPC, dt);
    }

    printf("[strength reduct opt ispc]:\t[%.3f] million cycles\n", minISPC);
    
    // Clear out the buffer
    for (unsigned int i = 0; i < length; ++i)
        values[i] = 0;
        
    //
    // Compute the hailstone sequence using the ispc implementation; report the minimum
    // time of three runs.
    //
    //double minISPC = 1e30;
    for (unsigned int i = 0; i < test_iterations[0]; ++i) {
        reset_and_start_timer();
        hailstone_ispc_controlflow(values, length);
        double dt = get_elapsed_mcycles();
        //printf("@time of ISPC run:\t\t\t[%.3f] million cycles\n", dt);
        minISPC = std::min(minISPC, dt);
    }
    
    printf("[control flow opt ispc]:\t[%.3f] million cycles\n", minISPC);
        
    // Clear out the buffer
    for (unsigned int i = 0; i < length; ++i)
        values[i] = 0;

    // 
    // And run the serial implementation 3 times, again reporting the
    // minimum time.
    //
    double minSerial = 1e30;
    for (unsigned int i = 0; i < test_iterations[1]; ++i) {
        reset_and_start_timer();
        hailstone_serial(values, length);
        double dt = get_elapsed_mcycles();
        //printf("@time of serial run:\t\t\t[%.3f] million cycles\n", dt);
        minSerial = std::min(minSerial, dt);
    }

    printf("[hailstone serial]:\t\t[%.3f] million cycles\n", minSerial);

    printf("\t\t\t\t(%.2fx speedup from ISPC)\n", minSerial/minISPC);

    return 0;
}
