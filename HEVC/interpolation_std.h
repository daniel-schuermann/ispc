#include <immintrin.h>

// *****
// std version, compiler vectorization only
// *****

template<typename Tsrc, bool shiftBack, bool biPred, bool bWeight, bool bChromaIntl>
static void qpelVC_scalar(const Tsrc* src, int srcStride, short *dst, int dstStride, int width, int height, int shift, int bitdepth, const short* coeff)//, wpPredParam *pwp1=NULL, wpPredParam *pwp2=NULL)
{
  int headroom = IF_INTERNAL_PREC - bitdepth;
  int shiftAvg = headroom +1;
  int offsetAvg =  1 << ( shiftAvg - 1 );
  int offset= 1 << (shift - 1);
  int iMaxVal = (1 << bitdepth) -1;
  
  src -= ( 4/2 - 1 ) * srcStride;

  const int wmod = bChromaIntl ? 2:1;
  int w0[2], w1[2], offsetW[2], shiftW;
  
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      int sum;
      sum  = src[ col + 0 * srcStride] * coeff[0];
      sum += src[ col + 1 * srcStride] * coeff[1];
      sum += src[ col + 2 * srcStride] * coeff[2];
      sum += src[ col + 3 * srcStride] * coeff[3];

      if ( shiftBack ){
        dst[col] = Clip3(0, iMaxVal, ( sum + offset ) >> shift );
      } 
      else {
        short val = sum >> shift;
        if (bWeight){
          if ( biPred ){
            dst[col] = Clip3(0, iMaxVal, (w0[col%wmod]*dst[col] + w1[col%wmod]*val + offsetW[col%wmod] ) >> shiftW );
          } 
	  else {
            dst[col] = Clip3(0, iMaxVal, (w0[col%wmod]*val + offsetW[col%wmod] ) >> shiftW );
          }
        } 
	else if (biPred){
	  // this case is not mapped properly, as dst does not contain the original kernel data
          dst[col] = Clip3(0, iMaxVal, (dst[col] + val + offsetAvg) >> shiftAvg );
        }
        else {
          dst[col] = val;
        }
      }
    }
    src += srcStride;
    dst += dstStride;
  }
}


