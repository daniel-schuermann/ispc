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
#include "grayscale_ispc.h"
#include <string.h>
#include <cstdlib>
using namespace ispc;


extern void tograyscale_serial(unsigned char * graybuf, unsigned char * buf, int w, int h);

struct image {
    unsigned char * buf;
    int width;
    int heigth;
}img;


#define PPMREADBUFLEN 256
int get_ppm(FILE *pf)
{
        char buf[PPMREADBUFLEN], *t;
        unsigned int w, h, d;
        int r;
 
        if (pf == NULL) return 1;
        t = fgets(buf, PPMREADBUFLEN, pf);
        /* the code fails if the white space following "P6" is not '\n' */
        if ( (t == NULL) || ( strncmp(buf, "P6\n", 3) != 0 ) ) return 1;
        do
        { /* Px formats can have # comments after first line */
           t = fgets(buf, PPMREADBUFLEN, pf);
           if ( t == NULL ) return 1;
        } while ( strncmp(buf, "#", 1) == 0 );
        r = sscanf(buf, "%u %u", &w, &h);
        if ( r < 2 ) return 1;
 
        r = fscanf(pf, "%u", &d);
        if ( (r < 1) || ( d != 255 ) ) return 1;
        fseek(pf, 1, SEEK_CUR); /* skip one byte, should be whitespace */
 
        img.buf = new unsigned char[w*h*3];
        if ( img.buf != NULL )
        {
            size_t rd = fread(img.buf, 3, w*h, pf);
            if ( rd < w*h )
            {
               free(img.buf);
               return 1;
            }
            img.width = w;
            img.heigth = h;
            return 0;
        } else
            return 1;
}

/* Write a PPM image file with the image of the Mandelbrot set */
static void
write_pgm(unsigned char *buf, int width, int height, const char *fn) {
    FILE *fp = fopen(fn, "wb");
    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int i = 0; i < width*height; ++i) {
        fputc(buf[i], fp);
    }
    fclose(fp);
    printf("Wrote image file %s\n", fn);
}



int main(int argc, char *argv[]) {
    FILE *fp = fopen("underwater_bmx.ppm", "r");
    if(get_ppm(fp)) return 1;
    
    unsigned char * graybuf = new unsigned char[img.width * img.heigth];
    
    
    static unsigned int test_iterations[] = {3, 3};



    //
    // Compute the image using the ispc implementation; report the minimum
    // time of three runs.
    //
    double minISPC = 1e30;
    for (unsigned int i = 0; i < test_iterations[0]; ++i) {
        reset_and_start_timer();
        tograyscale_ispc(graybuf, img.buf, img.width, img.heigth);
        double dt = get_elapsed_mcycles();
        printf("@time of ISPC run:\t\t\t[%.3f] million cycles\n", dt);
        minISPC = std::min(minISPC, dt);
    }

    printf("[grayscale ispc]:\t\t[%.3f] million cycles\n", minISPC);
    write_pgm(graybuf, img.width, img.heigth, "grayscale_ispc.pgm");

    // Clear out the buffer
    for (unsigned int i = 0; i < img.width * img.heigth; ++i)
        graybuf[i] = 0;

    // 
    // And run the serial implementation 3 times, again reporting the
    // minimum time.
    //
    double minSerial = 1e30;
    for (unsigned int i = 0; i < test_iterations[1]; ++i) {
        reset_and_start_timer();
        tograyscale_serial(graybuf, img.buf, img.width, img.heigth);
        double dt = get_elapsed_mcycles();
        printf("@time of serial run:\t\t\t[%.3f] million cycles\n", dt);
        minSerial = std::min(minSerial, dt);
    }

    printf("[grayscale serial]:\t\t[%.3f] million cycles\n", minSerial);
    write_pgm(graybuf, img.width, img.heigth, "grayscale_serial.pgm");

    printf("\t\t\t\t(%.2fx speedup from ISPC)\n", minSerial/minISPC);

    return 0;
}
