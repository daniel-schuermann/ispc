// ******
// Small Functions
// ******



static inline int16 Clip3_ispc(int minVal, int maxVal, int v)
{
  return min(max(minVal, v), maxVal);
}

static inline unmasked int qpel_h(uniform const int16 src[], int col, uniform int cstep, int c0, int c1, int c2, int c3)
{
  int s0,s1,s2,s3,sum;
  s0 = src[ col + cstep*0];
  s1 = src[ col + cstep*1];
  s2 = src[ col + cstep*2];
  s3 = src[ col + cstep*3];
  sum = s0*c0 + s1*c1 + s2*c2 + s3*c3;
  return sum;
}

// ******
// qpelC
// ******

static inline unmasked int16 qpelC_store(const int32 sum,
				const int16 old_val,
				const int32 index,
				const uniform bool  shiftBack,
				const uniform bool  bWeight,
				const uniform bool  biPred,				 
				const uniform int32 iMaxVal,
				const uniform int32 offset,
				const uniform int32 shift_param,
				const uniform int32 w0[2],
				const uniform int32 w1[2],
				const uniform int32 wmod,
				const uniform int32 offsetW[2],
				const uniform int32 shiftW,
				const uniform int32 offsetAvg,
				const uniform int32 shiftAvg)
{
  if ( shiftBack ){
    return clamp(( sum + offset ) >> shift_param, 0, iMaxVal);
  } else {
    // replace short with int, as there might be problems otherwise
    //int16 val = sum >> shift_param;
    int32 val = sum >> shift_param;
    if (bWeight){
      if ( biPred ){
        if(wmod == 1)
	return clamp((w0[0]*old_val + w1[0]*val + offsetW[0] ) >> shiftW, 0, iMaxVal );
        if(wmod == 2)
	return clamp((w0[index&1]*old_val + w1[index&1]*val + offsetW[index&1] ) >> shiftW, 0, iMaxVal );
      } else {
        if(wmod == 1)
	return clamp((w0[0]*val + offsetW[0] ) >> shiftW, 0, iMaxVal );
	      if(wmod == 2)
	return clamp((w0[index&1]*val + offsetW[index&1] ) >> shiftW, 0, iMaxVal );
      }
    } else if (biPred){
      return clamp((old_val + val + offsetAvg) >> shiftAvg, 0, iMaxVal );
    }
    else {
      return val;
    }
  }
}


static inline int16 qpelC_store_orig(const int32 sum,
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
	return Clip3_ispc(0, iMaxVal, (w0[index%wmod]*old_val + w1[index%wmod]*val + offsetW[index%wmod] ) >> shiftW );
      } else {
	return Clip3_ispc(0, iMaxVal, (w0[index%wmod]*val + offsetW[index%wmod] ) >> shiftW );
      }
    } else if (biPred){
      return Clip3_ispc(0, iMaxVal, (old_val + val + offsetAvg) >> shiftAvg );
    }
    else {
      return val;
    }
  }
}


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
  //foreach (row = 0...height, col = 0 ... width) {
  for(uniform int row = 0; row<height; ++row) {
    for(int col = programIndex; col<width; col+=programCount) {
      int sum = qpel_h(src, col, cstep, coeff[0], coeff[1], coeff[2], coeff[3]);
      dst[col] = clamp(( sum + offset ) >> shift_param, 0, iMaxVal);
    }
    dst += dstStride;
    src += srcStride;
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
  for(uniform int row = 0; row<height; ++row) {
    for(int col = programIndex; col<width; col+=programCount) {
      int sum = qpel_h(src, col, cstep, coeff[0], coeff[1], coeff[2], coeff[3]);
      dst[col] = clamp((w0[col&1]*dst[col] + w1[col&1]*(sum >> shift_param) + offsetW[col&1] ) >> shiftW, 0, iMaxVal );
    }
    dst += dstStride;
    src += srcStride;
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
  for(uniform int row = 0; row<height; ++row) {
    for(int col = programIndex; col<width; col+=programCount) {
      int sum = qpel_h(src, col, cstep, coeff[0], coeff[1], coeff[2], coeff[3]);
      dst[col] = clamp((w0[0]*dst[col] + w1[0]*(sum >> shift_param) + offsetW[0] ) >> shiftW, 0, iMaxVal );
    }
    dst += dstStride;
    src += srcStride;
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
  for(uniform int row = 0; row<height; ++row) {
    for(int col = programIndex; col<width; col+=programCount) {
      int sum = qpel_h(src, col, cstep, coeff[0], coeff[1], coeff[2], coeff[3]);
      dst[col] = clamp((w0[col&1]*(sum >> shift_param) + offsetW[col&1] ) >> shiftW, 0, iMaxVal );
    }
    dst += dstStride;
    src += srcStride;
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
  for(uniform int row = 0; row<height; ++row) {
    for(int col = programIndex; col<width; col+=programCount) {
      int sum = qpel_h(src, col, cstep, coeff[0], coeff[1], coeff[2], coeff[3]);
      dst[col] = clamp((w0[0]*(sum >> shift_param) + offsetW[0] ) >> shiftW, 0, iMaxVal );
    }
    dst += dstStride;
    src += srcStride;
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
  for(uniform int row = 0; row<height; ++row) {
    for(int col = programIndex; col<width; col+=programCount) {
      int sum = qpel_h(src, col, cstep, coeff[0], coeff[1], coeff[2], coeff[3]);
      dst[col] = clamp((dst[col] + (sum >> shift_param) + offsetAvg) >> shiftAvg, 0, iMaxVal );
    }
    dst += dstStride;
    src += srcStride;
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
  for(uniform int row = 0; row<height; ++row) {
    for(int col = programIndex; col<width; col+=programCount) {
      int sum = qpel_h(src, col, cstep, coeff[0], coeff[1], coeff[2], coeff[3]);
      dst[col] = sum >> shift_param;
    }
    dst += dstStride;
    src += srcStride;
  }
}

export static inline void qpel_ispc_h(uniform const int16 src[],
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
    
    int c[4];
    
    for (int i = 0; i < 4; i++){
      c[i] = (int) coeff[i];
    }

    int sum = 0;
    sum += src[ src_index + cstep*0] * c[0];
    sum += src[ src_index + cstep*1] * c[1];
    sum += src[ src_index + cstep*2] * c[2];
    sum += src[ src_index + cstep*3] * c[3];
    
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
    
    int c[4];
    
    for (int i = 0; i < 4; i++){
      c[i] = (int) coeff[i];
    }

    int sum = 0;
    sum += src[ src_index + cstep*0] * c[0];
    sum += src[ src_index + cstep*1] * c[1];
    sum += src[ src_index + cstep*2] * c[2];
    sum += src[ src_index + cstep*3] * c[3];
    
    dst[dst_index] = qpelC_store_orig(sum, dst[dst_index], dst_index, shiftBack, bWeight, biPred, iMaxVal, offset, shift_param, w0, w1, wmod, offsetW, shiftW, offsetAvg, shiftAvg);
  }
}


export void qpel_ispc_inlined(uniform const int16 src[],
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
  if ( shiftBack ){
    qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, true, false, false, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
  } else {
    if (bWeight){
      if ( biPred ){
        if(bChromaIntl)
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, true, true, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
        else
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, true, true, cstep, iMaxVal, offset, w0, w1, 1, offsetW, shiftW, offsetAvg, shiftAvg);
      } else {
        if(bChromaIntl)
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, false, true, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
        else
          qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, false, true, cstep, iMaxVal, offset, w0, w1, 1, offsetW, shiftW, offsetAvg, shiftAvg);
      }
    } else if (biPred){
        qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, true, false, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
    }
    else {
        qpel_ispc_h(src, srcStride, dst, dstStride, width, height, shift_param, bitdepth, coeff, false, false, false, cstep, iMaxVal, offset, w0, w1, 2, offsetW, shiftW, offsetAvg, shiftAvg);
    }
  }
}

/*
// original
ispc:	min speedup: 0.7	at index 205	max speedup: 1.1	at index 24	avg: 0.82
// strength reduction
ispc:	min speedup: 0.65	at index 84	max speedup: 1.2	at index 77	avg: 0.84
// inlined + strength reduction
ispc:	min speedup: 0.6	at index 213	max speedup: 1.1	at index 137	avg: 0.81
// rewritten
ispc:	min speedup: 0.81	at index 144	max speedup: 1.4	at index 200	avg: 0.99
*/
