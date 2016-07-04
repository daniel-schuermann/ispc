#ifndef HEVC_FUNCS_H
#define HEVC_FUNCS_H

#define  NO_OF_FRAMES  8

#define NTAPS_LUMA        8 ///< Number of taps for luma
#define NTAPS_CHROMA      4 ///< Number of taps for chroma
#define IF_INTERNAL_PREC 14 ///< Number of bits for internal precision
#define IF_FILTER_PREC    6 ///< Log2 of sum of filter taps
#define IF_INTERNAL_OFFS (1<<(IF_INTERNAL_PREC-1)) ///< Offset used internally

enum VEXT{SCALAR=0, SSE2, SSE3, SSSE3_SLM, SSSE3, SSE41, SSE42, AVX, XOP, AVX2, AUTO};

const VEXT MY_VEXT = AVX;
/*
// scalar and auto-vectorized versions
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_scalar(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
//template <VEXT vext, typename Tsrc, bool shiftBack, bool biPred> void qpelVC_intr(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);

// implicit version
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_cilk(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_cilkarray(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_openmp(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);

//explict versions
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_boost(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_gsimd(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_vc(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);

// hybrid versions
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_ispc(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_sierra(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff);
*/

// helper function
/** clip a, such that minVal <= a <= maxVal */
template <typename T>
inline T Clip3( T minVal, T maxVal, T v) {
  return std::min<T> (std::max<T> (minVal, v) , maxVal);
}

#include "interpolation_std.h"
#include "interpolation_hybrid.h"
#endif
