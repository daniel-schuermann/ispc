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
    
    dst[dst_index] = qpelC_store(sum, dst[dst_index], dst_index, shiftBack, bWeight, biPred, iMaxVal, offset, shift_param, w0, w1, wmod, offsetW, shiftW, offsetAvg, shiftAvg);
  }
}

