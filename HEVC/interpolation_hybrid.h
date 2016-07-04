// *****
// ipsc Wrapper
// *****

#include "interpolation_ispc.h"

template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_ispc_original(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff)
{

  int headroom = IF_INTERNAL_PREC - bitdepth;
  int shiftAvg = headroom +1;
  int offsetAvg =  1 << ( shiftAvg - 1 );
  int offset= 1 << (shift - 1);
  int iMaxVal = (1 << bitdepth) -1;
  
  src -= ( 4/2 - 1 ) * srcStride;

  const int wmod = bChromaIntl ? 2:1;
  int w0[2], w1[2], offsetW[2], shiftW;
  
  // original
  ispc::qpel_ispc((short*) src, srcStride, dst, dstStride, width, height, shift, bitdepth, coeff, shiftBack, biPred, bWeight, srcStride, iMaxVal, offset, w0, w1, wmod, offsetW, shiftW, offsetAvg, shiftAvg);

}

template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_ispc_opt(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff)
{

  int headroom = IF_INTERNAL_PREC - bitdepth;
  int shiftAvg = headroom +1;
  int offsetAvg =  1 << ( shiftAvg - 1 );
  int offset= 1 << (shift - 1);
  int iMaxVal = (1 << bitdepth) -1;
  
  src -= ( 4/2 - 1 ) * srcStride;

  const int wmod = bChromaIntl ? 2:1;
  int w0[2], w1[2], offsetW[2], shiftW;
  
  // strength reduction
  ispc::qpel_ispc_h((short*) src, srcStride, dst, dstStride, width, height, shift, bitdepth, coeff, shiftBack, biPred, bWeight,  srcStride, iMaxVal, offset, w0, w1, wmod, offsetW, shiftW, offsetAvg, shiftAvg);

}

template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_ispc_inlined_opt(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff)
{

  int headroom = IF_INTERNAL_PREC - bitdepth;
  int shiftAvg = headroom +1;
  int offsetAvg =  1 << ( shiftAvg - 1 );
  int offset= 1 << (shift - 1);
  int iMaxVal = (1 << bitdepth) -1;
  
  src -= ( 4/2 - 1 ) * srcStride;

  const int wmod = bChromaIntl ? 2:1;
  int w0[2], w1[2], offsetW[2], shiftW;
  
  // inlined + const prop
  ispc::qpel_ispc_inlined((short*) src, srcStride, dst, dstStride, width, height, shift, bitdepth, coeff, shiftBack, biPred, bWeight, bChromaIntl,  srcStride, iMaxVal, offset, w0, w1, offsetW, shiftW, offsetAvg, shiftAvg);

}

template <typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl> void qpelVC_ispc_rewritten(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff)
{
  int headroom = IF_INTERNAL_PREC - bitdepth;
  int shiftAvg = headroom +1;
  int offsetAvg =  1 << ( shiftAvg - 1 );
  int offset= 1 << (shift - 1);
  int iMaxVal = (1 << bitdepth) -1;
  
  src -= ( 4/2 - 1 ) * srcStride;

  const int wmod = bChromaIntl ? 2:1;
  int w0[2], w1[2], offsetW[2], shiftW;
  
  // rewritten for templates
  if(shiftBack)
    ispc::qpel_shiftBack((short*) src, srcStride, dst, dstStride, width, height, shift, coeff, srcStride, iMaxVal, offset);
  else if(bWeight) {
    if(biPred) {
      if(bChromaIntl)
        ispc::qpel_bW_biP_Chr((short*) src, srcStride, dst, dstStride, width, height, shift, coeff, srcStride, iMaxVal, w0, w1, offsetW, shiftW);
      else
        ispc::qpel_bW_biP((short*) src, srcStride, dst, dstStride, width, height, shift, coeff, srcStride, iMaxVal, w0, w1, offsetW, shiftW);
    } else { // biWeight, !biPred
      if(bChromaIntl)
        ispc::qpel_bW_Chr((short*) src, srcStride, dst, dstStride, width, height, shift, coeff, srcStride, iMaxVal, w0, offsetW, shiftW);
      else
        ispc::qpel_bW((short*) src, srcStride, dst, dstStride, width, height, shift, coeff, srcStride, iMaxVal, w0, offsetW, shiftW);
    }
  } else { // not bWeight
    if(biPred) {
      ispc::qpel_biPred((short*) src, srcStride, dst, dstStride, width, height, shift, coeff, srcStride, iMaxVal, offsetAvg, shiftAvg);
    } else {
      ispc::qpel_nothing((short*) src, srcStride, dst, dstStride, width, height, shift, coeff, srcStride);
    }
  }
}


