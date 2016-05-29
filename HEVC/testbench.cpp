#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <stdint.h>
#include <numeric>
#include <typeinfo>

#include "timing.h"
#include "hevc_funcs.h"

// enum type for all different libraries, for easier array accesses
enum Versions {Scalar = 0, Ispc, Intr, NO_OF_VERS, Cilk, Cilkarray, Openmp, Boost, Gsimd, Vclib, Sierra};

// small function for results printing
std::string getName(int version)
{ 
  switch (version){
  case 0: return "Scalar";
  case 1: return "ispc";//Cilk";
  case 2: return "Intrinsics";
  case 3: return "OpenMP";
  case 4: return "Boost";
  case 5: return "GSimd";
  case 6: return "Vc";
  case 7: return "ispc";
  case 8: return "Sierra";
  case 9: return "Intrinsics";
  default:  return "UNDEFINED";
  }
}

// file read function
template<typename destType>
void read_file(const char* infile, destType* valarray, int size)
{
  using std::ifstream;
  ifstream fin;

  fin.open(infile, std::ios_base::in);
  for (int i = 0; i < size; i ++)
    {
      fin >> valarray[i];
    }
}

template<typename destType>
void read_line(const char* infile, destType* valarray, int size, int offset)
{
  using std::ifstream;
  ifstream fin;

  std::string line;
  int line_cnt = 0;
  fin.open(infile, std::ios_base::in);  
  while ((std::getline(fin, line)) && (line_cnt < offset))
    {
      line_cnt ++;
    }
  // seperate string into destType
  std::istringstream is(line);
  int i = 0;
  while (i < size)
    {
      is >> valarray[i];
      i++;
    }
}

int main(int argc, char *argv[]){

  // file reading
  int params[(10<<NO_OF_FRAMES)];
  for(int i = 0; i<10<<NO_OF_FRAMES; i++)
        params[i] = 0;
  int* param_ptr = &params[0];
  read_file<int>("traces/param.trace", param_ptr, (10<<NO_OF_FRAMES));
  
  short coeffs[(8<<NO_OF_FRAMES)];
  for(int i = 0; i<8<<NO_OF_FRAMES; i++)
        coeffs[i] = 0;
  short* coeff_ptr = &coeffs[0];
  read_file<short>("traces/coeff.trace", coeff_ptr, (8<<NO_OF_FRAMES));

  double times[NO_OF_VERS][(1<<NO_OF_FRAMES)];
  float speedups[NO_OF_VERS][(1<<NO_OF_FRAMES)];

  for (int frame = 0; frame < (1<<NO_OF_FRAMES); frame++){

    bool shiftBack   = params[10*frame+0];
    bool biPred      = params[10*frame+1];
    bool bWeight     = params[10*frame+2];
    bool bChromaIntl = params[10*frame+3];
    int srcStride    = params[10*frame+4];
    int dstStride    = params[10*frame+5];
    int width        = params[10*frame+6];
    int height       = params[10*frame+7];
    //int width       = 50;
    //int height      = 50;
    int shift        = params[10*frame+8];
    char bitdepth    = params[10*frame+9];

    // calculate src size from parameters
    //    int src_size = height * (srcStride + width);
    int src_size = height * width * srcStride;
    // and read from file
    short src[src_size];
    for(int i = 0; i<src_size; i++)
        src[i] = 0;
    short* src_ptr = &src[0];
    read_line<short>("traces/src.trace", src_ptr, src_size, frame);
    // adjust pointer for easier results checking
    src_ptr += ( 4/2 - 1 ) * srcStride;

    // calculate number of outputs
    int dst_size = dstStride * height;
    short my_dest[dst_size];
    short* dstPtr = &my_dest[0];    
    // for debugging
    short* dst_copy = &my_dest[0];

    // ugly hack to create parameter vector: 
    bool param_vec[4] = {shiftBack, biPred, bWeight, bChromaIntl};
    char pvec = 0;
    for (int m =0; m < 4; m++) pvec = (pvec<<1) | param_vec[m];

    // check template parameters and set function pointer accordingly:
    void (*fct_ptr[NO_OF_VERS-1])       (const short* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
    // intrinsics has less function arguments, hence, its own function pointer
    void (*fct_ptr_intr)  (const short* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);

    switch (pvec){
    case (0): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,0,0,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,0,0,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,0,0,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,0,0,0>;
      //fct_ptr[Boost] = qpelVC_boost<short, 0,0,0,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,0,0,0>; fct_ptr[Vclib] = qpelVC_vc<short, 0,0,0,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,0,0,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,0,0,0>;
#endif
      break;
    case (1): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,0,0,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,0,0,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,0,0,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,0,0,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 0,0,0,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,0,0,1>; fct_ptr[Vclib] = qpelVC_vc<short, 0,0,0,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,0,0,1>; 
#ifdef SIERRA
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,0,0,1>;
#endif
      break;
    case (2): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,0,1,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,0,1,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,0,1,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,0,1,0>; 
      //fct_ptr[Boost] = qpelVC_boost<short, 0,0,1,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,0,1,0>; fct_ptr[Vclib] = qpelVC_vc<short, 0,0,1,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,0,1,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,0,1,0>;
#endif
      break;
    case (3): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,0,1,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,0,1,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,0,1,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,0,1,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 0,0,1,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,0,1,1>; fct_ptr[Vclib] = qpelVC_vc<short, 0,0,1,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,0,1,1>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,0,1,1>;
#endif
      break;
    case (4): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,1,0,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,1,0,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,1,0,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,1,0,0>;
      //fct_ptr[Boost] = qpelVC_boost<short, 0,1,0,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,1,0,0>; fct_ptr[Vclib] = qpelVC_vc<short, 0,1,0,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,1,0,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,1,0,0>;
#endif
      break; 
    case (5): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,1,0,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,1,0,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,1,0,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,1,0,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 0,1,0,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,1,0,1>; fct_ptr[Vclib] = qpelVC_vc<short, 0,1,0,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,1,0,1>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,1,0,1>;
#endif
      break;
    case (6): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,1,1,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,1,1,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,1,1,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,1,1,0>;
      //fct_ptr[Boost] = qpelVC_boost<short, 0,1,1,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,1,1,0>; fct_ptr[Vclib] = qpelVC_vc<short, 0,1,1,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,1,1,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,1,1,0>;
#endif
      break;
    case (7): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 0,1,1,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,0,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 0,1,1,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 0,1,1,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 0,1,1,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 0,1,1,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 0,1,1,1>; fct_ptr[Vclib] = qpelVC_vc<short, 0,1,1,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 0,1,1,1>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 0,1,1,1>;
#endif
      break;
    case (8): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,0,0,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,0,0,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,0,0,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,0,0,0>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,0,0,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,0,0,0>; fct_ptr[Vclib] = qpelVC_vc<short, 1,0,0,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,0,0,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,0,0,0>;
#endif
      break;
    case (9): 
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,0,0,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,0,0,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,0,0,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,0,0,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,0,0,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,0,0,1>; fct_ptr[Vclib] = qpelVC_vc<short, 1,0,0,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,0,0,1>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,0,0,1>;
#endif
      break;
    case (10):
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,0,1,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,0,1,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,0,1,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,0,1,0>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,0,1,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,0,1,0>; fct_ptr[Vclib] = qpelVC_vc<short, 1,0,1,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,0,1,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,0,1,0>;
#endif
      break;
    case (11):
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,0,1,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,0>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,0,1,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,0,1,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,0,1,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,0,1,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,0,1,1>; fct_ptr[Vclib] = qpelVC_vc<short, 1,0,1,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,0,1,1>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,0,1,1>;
#endif
      break;
    case (12):
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,1,0,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,1,0,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,1,0,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,1,0,0>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,1,0,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,1,0,0>; fct_ptr[Vclib] = qpelVC_vc<short, 1,1,0,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,1,0,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,1,0,0>;
#endif
      break;
    case (13):
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,1,0,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,1,0,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,1,0,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,1,0,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,1,0,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,1,0,1>; fct_ptr[Vclib] = qpelVC_vc<short, 1,1,0,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,1,0,1>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,1,0,1>;
#endif
      break;
    case (14):
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,1,1,0>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,1,1,0>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,1,1,0>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,1,1,0>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,1,1,0>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,1,1,0>; fct_ptr[Vclib] = qpelVC_vc<short, 1,1,1,0>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,1,1,0>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,1,1,0>;
#endif
      break;
    default:  
      fct_ptr[Scalar]= qpelVC_scalar<short, 1,1,1,1>; fct_ptr_intr = qpelVC_intr<MY_VEXT,short,1,1>;
      //fct_ptr[Cilk]= qpelVC_cilk<short, 1,1,1,1>; fct_ptr[Cilkarray] = qpelVC_cilkarray<short, 1,1,1,1>; fct_ptr[Openmp] = qpelVC_openmp<short, 1,1,1,1>;
      //fct_ptr[Boost] = qpelVC_boost<short, 1,1,1,1>; fct_ptr[Gsimd] = qpelVC_gsimd<short, 1,1,1,1>; fct_ptr[Vclib] = qpelVC_vc<short, 1,1,1,1>;
      fct_ptr[Ispc] = qpelVC_ispc<short, 1,1,1,1>; 
#ifdef SIERRA 
      //fct_ptr[Sierra] = qpelVC_sierra<short, 1,1,1,1>;
#endif
      break;
    }
   
    // setup timer
    uint64_t start, end;
    double tmp;

    // intrinsics
    // average measurement for 100 runs:
    tmp = 0;
    for (int t = 0; t < 1000; t++){
      start = rdtsc();
      fct_ptr_intr(src_ptr, srcStride, dstPtr, dstStride, width, height, shift, bitdepth, &coeff_ptr[8*frame]);
      end = rdtsc();
      tmp += (end-start);
    }
    times[Intr][frame] = tmp / 1000;

    // check if all functions should be executed
    int bound = Intr;
#ifdef SIERRA
  //  bound = NO_OF_VERS;
#else
  //  bound = Sierra;
#endif

    // all other versions
    for (int i = Scalar; i < bound; i++){
      tmp = 0;
      for (int t = 0; t < 1000; t++){
	start = rdtsc();
	fct_ptr[i](src_ptr, srcStride, dstPtr, dstStride, width, height, shift, bitdepth, &coeff_ptr[8*frame]);
	end = rdtsc();
	tmp += (end-start);
      }
      times[i][frame] = tmp / 1000;
    }
    
    // calculate avg speedups
    for (int i = Scalar; i < bound+1; i++)
      speedups[i][frame]        = times[Scalar][frame]/times[i][frame];

    // let's check the results
    // read reference numbers
    int res_size = width * height;
    short results[res_size];
    for(int i = 0; i<res_size; i++)
        results[i] = 0;
    short* res_ptr = &results[0];
    read_line<short>("traces/dest.trace", res_ptr, res_size, frame);
    
    int index = 0;
    bool error_flag = false;
    for (int j = 0; j < height; j++)
      {
	for (int k = 0; k < width; k++){
	  if (dst_copy[k] != results[index])
	    {
	      error_flag = true;
	    }
	  index++;      
	}      
	dst_copy += dstStride;
      }
    
    if (error_flag){
      printf("Frame %d completed with errors!\tScalar: %.2f\tIntrinsics: %.2f\tCilk: %.2f\tCilkArray: %.2f\tOpenMP: %.2f\tBoost: %.2f\tGSimd: %.2f\tVc: %.2f\tispc: %.2f\n", frame, times[Scalar][frame],times[Intr][frame], times[Cilk][frame], times[Cilkarray][frame],  times[Openmp][frame], times[Boost][frame], times[Gsimd][frame], times[Vclib][frame], times[Ispc][frame]);
    }
    else{
      printf("Frame %d completed successfully!\tScalar: %.2f\tIntrinsics: %.2f\tispc: %.2f\n", frame, times[Scalar][frame],times[Intr][frame], times[Ispc][frame]);
    }
  }

  // print out longest frame to check for improvements
  const int size = sizeof(times[Intr])/sizeof(double);

  // check speed-ups:
  float speedup_min[NO_OF_VERS-1];
  int index_min[NO_OF_VERS-1];
  float speedup_max[NO_OF_VERS-1];
  int index_max[NO_OF_VERS-1];
  float speedup_avg[NO_OF_VERS-1];

  // speedups are not calculated for scalar value, hence loop starts at next enum item
  for (int i = Scalar+1; i < NO_OF_VERS; i++){
    speedup_min[i] = *std::min_element(speedups[i], speedups[i]+size);
    index_min[i] = std::distance(speedups[i], std::min_element(speedups[i], speedups[i]+size));
    speedup_max[i] = *std::max_element(speedups[i], speedups[i]+size);
    index_max[i] = std::distance(speedups[i], std::max_element(speedups[i], speedups[i]+size));
    speedup_avg[i] = std::accumulate(speedups[i], speedups[i]+size, 0.0)/(float)size;
    std::cout << std::setprecision(2) << getName(i) << ":\tmin speedup: " << speedup_min[i] << "\tat index " << index_min[i] << "\tmax speedup: " << speedup_max[i] << "\tat index " << index_max[i] << "\tavg: " << speedup_avg[i] << std::endl;
  }
}
