#!/bin/sh

# This is a temporary, simple build script until twrp is ready for anykernel

export ARCH=arm64
export SUBARCH=arm64
export CLANG_PATH=~/android/rom/oreo/prebuilts/clang/host/linux-x86/clang-3957855/bin/
export CLANG_TRIPLE=aarch64-linux-gnu-
export CROSS_COMPILE=~/android/kernel/aarch64-linaro-linux-gnu/bin/aarch64-linux-gnu-

make mrproper -j8
make electron_defconfig
make PATH=${CLANG_PATH}:${PATH} CC=clang -j8
