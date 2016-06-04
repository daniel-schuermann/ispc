
EXAMPLE=grayscale
CPP_SRC=grayscale.cpp grayscale_serial.cpp
ISPC_SRC=grayscale.ispc
ISPC_IA_TARGETS=avx1-i32x16 --explain=2#sse2-i32x4,sse4-i32x8,avx1-i32x16,avx2-i32x16,avx512knl-i32x16#,avx512skx-i32x16
ISPC_ARM_TARGETS=neon

include ../common.mk
