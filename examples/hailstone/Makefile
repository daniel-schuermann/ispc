
EXAMPLE=hailstone
CPP_SRC=hailstone.cpp hailstone_serial.cpp
ISPC_SRC=hailstone.ispc
ISPC_IA_TARGETS= host --explain=2#sse4a-i32x8#,sse2-i32x4,avx1-i32x16,avx2-i32x16,avx512knl-i32x16,avx512skx-i32x16
ISPC_ARM_TARGETS=neon

include ../common.mk
