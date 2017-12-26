# AnyKernel2 Ramdisk Mod Script
# osm0sis @ xda-developers

## AnyKernel setup
# begin properties
properties() {
do.devicecheck=1
do.modules=0
do.cleanup=1
do.cleanuponabort=0
device.name1=taimen

} # end properties

# shell variables
block=/dev/block/bootdevice/by-name/boot;
is_slot_device=1;
ramdisk_compression=auto;

## AnyKernel methods (DO NOT CHANGE)
# import patching functions/variables - see for reference
. /tmp/anykernel/tools/ak2-core.sh;


## AnyKernel file attributes
# set permissions/ownership for included ramdisk files
chmod -R 750 $ramdisk/*;
chown -R root:root $ramdisk/*;


## AnyKernel install
dump_boot;

# Disable AVB
replace_string /vendor/etc/fstab.taimen "slotselect " "slotselect,avb" "slotselect"

# Include init.electron.rc
insert_line /vendor/etc/init/hw/init.taimen.rc "electron" after 'import /vendor/etc/init/hw/init.${ro.hardware}.usb.rc' "import /vendor/etc/init/hw/init.electron.rc"

write_boot;

## end install

