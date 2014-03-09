/opt/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc \
 -MMD -MP -MF \
-fpic -ffunction-sections -funwind-tables  -fstack-protector\
 -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__   \
-Wno-psabi -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -mthumb -Os -fomit-frame-pointer \
-fno-strict-aliasing -finline-limit=64 -mfpu=neon -I/usr/local/include \
-I/media/UBUNTU/repository/OpenVisionLibrary/OpenVision/ -I/opt/android-ndk-r7/sources//android/cpufeatures \
-I/opt/android-ndk-r7/sources/cxx-stl/gnu-libstdc++/include -I/opt/android-ndk-r7/sources/cxx-stl/gnu-libstdc++/libs/armeabi-v7a/include \
-I/home/pi19404/ARM//jni -DANDROID -DHAVE_NEON -fPIC -DANDROID -I/usr/local/include/opencv -I/usr/local/include -I/OpenVision \
-I/media/UBUNTU/repository/OpenVisionLibrary/OpenVision -fPIC -DHAVE_NEON=1 -ftree-vectorize \
-mfpu=neon -O3 -mfloat-abi=softfp -ffast-math -Wa,--noexecstack -O3 -DNDEBUG \
 -I/opt/android-ndk-r7/platforms/android-8/arch-arm/usr/include -c /home/pi19404/ARM/jni/helloneon-intrinsics.s \
 -o /home/pi19404/ARM/obj/local/armeabi-v7a/objs/helloneon/helloneon-intrinsics.o --sysroot=/opt/android-ndk-r7/platforms/android-14/arch-arm/ 

/opt/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-g++ -Wl,--gc-sections -Wl,\
-z,nocopyreloc --sysroot=/opt/android-ndk-r7/platforms/android-8/arch-arm  /home/pi19404/ARM//obj/local/armeabi-v7a/objs/helloneon/neon.o \
/home/pi19404/ARM//obj/local/armeabi-v7a/objs/helloneon/helloneon-intrinsics.o   /home/pi19404/ARM//obj/local/armeabi-v7a/libcpufeatures.a\
 /home/pi19404/ARM//obj/local/armeabi-v7a/libgnustl_static.a \
/opt/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.4.3/libgcc.a   \
 -Wl,--fix-cortex-a8  -Wl,--no-undefined -Wl,-z,noexecstack -L/opt/android-ndk-r7/platforms/android-8/arch-arm/usr/lib -fPIC -llog \
 -ldl -lm -lz -lm -lc -lgcc -Wl,-rpath,'libs/armeabi-v7a' -L/home/pi19404/ARM//jni/../libs/armeabi -llog -Llibs/armebi -Llibs/armeabi-v7a \
-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_flann -lc -lm -o /home/pi19404/ARM//obj/local/armeabi-v7a/helloneon

cp /home/pi19404/ARM//obj/local/armeabi-v7a/helloneon libs/armeabi-v7a
