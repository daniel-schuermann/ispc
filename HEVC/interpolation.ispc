// ******
// Small Functions
// ******

static inline int16 Clip3_ispc(int minVal, int maxVal, int v)
{
  return min(max(minVal, v), maxVal);
}

// ******
// qpelC
// ******
/*
static inline int16 qpelC_store(const int32 sum,
				const int16 old_val,
				const int32 index,
				const bool  shiftBack,
				const bool  bWeight,
				const bool  biPred,				 
				const int32 iMaxVal,
				const int32 offset,
				const int32 shift_param,
				const uniform int32 w0[2],
				const uniform int32 w1[2],
				const int32 wmod,
				const uniform int32 offsetW[2],
				const int32 shiftW,
				const int32 offsetAvg,
				const int32 shiftAvg)
{
  if ( shiftBack ){
    return Clip3_ispc(0, iMaxVal, ( sum + offset ) >> shift_param);
  } else {
    // replace short with int, as there might be problems otherwise
    //int16 val = sum >> shift_param;
    int32 val = sum >> shift_param;
    if (bWeight){
      if ( biPred ){
        if(wmod == 1)
	return Clip3_ispc(0, iMaxVal, (w0[0]*old_val + w1[0]*val + offsetW[0] ) >> shiftW );
        if(wmod == 2)
	return Clip3_ispc(0, iMaxVal, (w0[index&1]*old_val + w1[index&1]*val + offsetW[index&1] ) >> shiftW );
      } else {
        if(wmod == 1)
	return Clip3_ispc(0, iMaxVal, (w0[0]*val + offsetW[0] ) >> shiftW );
	      if(wmod == 2)
	return Clip3_ispc(0, iMaxVal, (w0[index&1]*val + offsetW[index&1] ) >> shiftW );
      }
    } else if (biPred){
      return Clip3_ispc(0, iMaxVal, (old_val + val + offsetAvg) >> shiftAvg );
    }
    else {
      return val;
    }
  }
}
*/
export static void qpel_shiftBack(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int16 coeff[4],
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 offset)
{
  foreach (row = 0...height, col = 0 ... width) {
    
    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];
    
    dst[dst_index] = Clip3_ispc(0, iMaxVal, ( sum + offset ) >> shift_param);
  }
}

export static void qpel_bW_biP_Chr(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int16 coeff[4],
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 w0[2],
			uniform const int32 w1[2],
			uniform const int32 offsetW[2],
			uniform const int32 shiftW)
{

  foreach (row = 0...height, col = 0 ... width) {
    
    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];
    
    dst[dst_index] = Clip3_ispc(0, iMaxVal, (w0[dst_index&1]*dst[dst_index] + w1[dst_index&1]*(sum >> shift_param) + offsetW[dst_index&1] ) >> shiftW );
  }
}

export static void qpel_bW_biP(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int16 coeff[4],
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 w0[2],
			uniform const int32 w1[2],
			uniform const int32 offsetW[2],
			uniform const int32 shiftW)
{

  foreach (row = 0...height, col = 0 ... width) {
    
    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];
    
    dst[dst_index] = Clip3_ispc(0, iMaxVal, (w0[0]*dst[dst_index] + w1[0]*(sum >> shift_param) + offsetW[0] ) >> shiftW );
  }
}

export static void qpel_bW_Chr(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int16 coeff[4],
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 w0[2],
			uniform const int32 offsetW[2],
			uniform const int32 shiftW)
{

  foreach (row = 0...height, col = 0 ... width) {
    
    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];
    
    dst[dst_index] = Clip3_ispc(0, iMaxVal, (w0[dst_index&1]*(sum >> shift_param) + offsetW[dst_index&1] ) >> shiftW );
  }
}

export static void qpel_bW(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int16 coeff[4],
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 w0[2],
			uniform const int32 offsetW[2],
			uniform const int32 shiftW)
{

  foreach (row = 0...height, col = 0 ... width) {
    
    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];
    
    dst[dst_index] = Clip3_ispc(0, iMaxVal, (w0[0]*(sum >> shift_param) + offsetW[0] ) >> shiftW );
  }
}

export static void qpel_biPred(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int16 coeff[4],
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 offsetAvg,
			uniform const int32 shiftAvg)
{
  foreach (row = 0...height, col = 0 ... width) {
    
    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];
    
    dst[dst_index] = Clip3_ispc(0, iMaxVal, (dst[dst_index] + (sum >> shift_param) + offsetAvg) >> shiftAvg );
  }
}

export static void qpel_nothing(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int16 coeff[4],
			// function param
			uniform const int32 cstep)
{

  foreach (row = 0...height, col = 0 ... width) {

    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];

    dst[dst_index] = sum >> shift_param;
  }
}
/*
static inline void qpel_ispc_h(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int32 bitdepth, 
			uniform const int16 coeff[4],
			// template param
			uniform const bool  shiftBack,
			uniform const bool  biPred,
			uniform const bool  bWeight,
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 offset,
			uniform const int32 w0[2],
			uniform const int32 w1[2],
			uniform const int32 wmod,
			uniform const int32 offsetW[2],
			uniform const int32 shiftW,
			uniform const int32 offsetAvg,
			uniform const int32 shiftAvg)
{

  foreach (row = 0...height, col = 0 ... width) {
    
    int dst_index = row * dstStride + col;
    int src_index = row * srcStride + col;
    
    int sum = 0;
    sum += src[ src_index + cstep*0] * (int)coeff[0];
    sum += src[ src_index + cstep*1] * (int)coeff[1];
    sum += src[ src_index + cstep*2] * (int)coeff[2];
    sum += src[ src_index + cstep*3] * (int)coeff[3];
    
    dst[dst_index] = qpelC_store(sum, dst[dst_index], dst_index, shiftBack, bWeight, biPred, iMaxVal, offset, shift_param, w0, w1, wmod, offsetW, shiftW, offsetAvg, shiftAvg);
  }
}


export void qpel_ispc(uniform const int16 src[],
			uniform const int32 srcStride,
			uniform int16 dst[],
			uniform const int32 dstStride,
			uniform const int32 width, 
			uniform const int32 height, 
			uniform const int32 shift_param, 
			uniform const int32 bitdepth, 
			uniform const int16 coeff[4],
			// template param
			uniform const bool  shiftBack,
			uniform const bool  biPred,
			uniform const bool  bWeight,
			uniform const bool  bChromaIntl,
			// function param
			uniform const int32 cstep,
			uniform const int32 iMaxVal,
			uniform const int32 offset,
			uniform const int32 w0[2],
			uniform const int32 w1[2],
			//uniform const int32 wmod,
			uniform const int32 offsetW[2],
			uniform const int32 shiftW,
			uniform const int32 offsetAvg,
			uniform const int32 shiftAvg)
{
  if(bChromaIntl) {
    if ( shiftBack ){
      qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, true, biPred, bWeight, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
    } else {
      if (bWeight){
        if ( biPred ){
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, true, true, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
        } else {
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, false, true, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
        }
      } else if (biPred){
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, true, false, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
      }
      else {
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, false, false, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
      }
    }
  } else {
    if ( shiftBack ){
      qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, true, biPred, bWeight, cstep, iMaxVal, offset, w0, w1, 1, offsetW, shiftW, offsetAvg, shiftAvg);
    } else {
      if (bWeight){
        if ( biPred ){
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, true, true, cstep, iMaxVal, offset, w0, w1, 1, offsetW, shiftW, offsetAvg, shiftAvg);
        } else {
        qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, false, true, cstep, iMaxVal, offset, w0, w1, 1, offsetW, shiftW, offsetAvg, shiftAvg);
        }
      } else if (biPred){
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, true, false, cstep, iMaxVal, offset, w0, w1, 1, offsetW, shiftW, offsetAvg, shiftAvg);
      }
      else {
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, false, false, cstep, iMaxVal, offset, w0, w1, 1, offsetW, shiftW, offsetAvg, shiftAvg);
      }
    }
  }
}*/
