#!/bin/sh

# This is a temporary, simple build script until twrp is ready for anykernel

export ARCH=arm64
export SUBARCH=arm64
export CLANG_PATH=~/android/rom/oreo/prebuilts/clang/host/linux-x86/clang-3957855/bin/
export CLANG_TRIPLE=aarch64-linux-gnu-
export CROSS_COMPILE=~/android/kernel/aarch64-linaro-linux-gnu/bin/aarch64-linux-gnu-

make mrproper -j8
make electron_defconfig
make PATH=${CLANG_PATH}:${PATH} CC="ccache clang" -j8

if [[ -n "$1" ]]; then
    rm ../img/device/google/wahoo-kernel/Image.lz4-dtb *.ko;
    cp arch/arm64/boot/Image.lz4-dtb ../img/device/google/wahoo-kernel/Image.lz4-dtb;
    find . -name *.ko | xargs -I{} cp {} ../img/device/google/wahoo-kernel/;
    cd ../img;
    . build/envsetup.sh;
    lunch aosp_taimen-user;
    make clean;
    make bootimage -j8;
fi;
