
export NDK_PROJECT_PATH=/home/pi19404/ARM
export TARGET_ARCH_ABI=armeabi-v7a
ndk-build V=1 all 
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
adb push libs/armeabi-v7a/ /data/local/tmp/NEON_TEST


echo "export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH" > a.ksh
echo "cd /data/local/tmp/NEON_TEST" >> a.ksh
echo "./helloneon" >> a.ksh
