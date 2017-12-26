#!/bin/sh

# This is a temporary, simple build script until twrp is ready for anykernel

export ARCH=arm64
export SUBARCH=arm64
export CLANG_TRIPLE=aarch64-linux-gnu-
export CLANG_PATH=~/android/kernel/dtc5/bin
export CROSS_COMPILE=~/android/kernel/aarch64-linaro-linux-gnu/bin/aarch64-linux-gnu-
export REL="v4"
if [ -n "$1" ]; then
    export TESTVER="-$1"
fi;
export LOCALVERSION="-Electron-$REL$TESTVER"
export COMPILER_NAME="DragonTC 5.0"

make mrproper -j8;
make electron_defconfig;
make CC="ccache $CLANG_PATH/clang" Image.lz4-dtb -j8;
#make CC="ccache $CLANG_PATH/clang" modules -j8;

rm ../img/device/google/wahoo-kernel/Image.lz4-dtb ../img/device/google/wahoo-kernel/*.ko;
cp arch/arm64/boot/Image.lz4-dtb ../img/device/google/wahoo-kernel/Image.lz4-dtb;
find . -name *.ko | xargs -I{} cp {} ../img/device/google/wahoo-kernel/;
cd ../img;
. build/envsetup.sh;
lunch aosp_taimen-user;
make installclean;
make bootimage -j8;
cd ../taimen;
rm -rf out;
mkdir out;
cp ~/android/kernel/img/out/target/product/taimen/boot.img ~/android/kernel/taimen/out/$LOCALVERSION.img;
