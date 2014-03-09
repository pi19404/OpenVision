# Build both ARMv5TE and ARMv7-A machine code.
APP_ABI :=   armeabi-v7a
APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -fexceptions -ftree-vectorize  -mfpu=neon -O3 -mfloat-abi=softfp -ffast-math



