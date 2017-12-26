#!bin/sh

IMG_DIR=$1
MOUNTPOINT="/tmp/ubi"
mkdir -p $MOUNTPOINT

ubiformat /dev/mtd1
ubiattach -m 1
ubimkvol /dev/ubi0 -N boot -m
mount -t ubifs /dev/ubi0_0 $MOUNTPOINT
cp -a $IMG_DIR/uImage $MOUNTPOINT
sync
umount $MOUNTPOINT


ubiformat /dev/mtd2
ubiattach -m 2
ubimkvol /dev/ubi1 -N rootfs -m
mount -t ubifs /dev/ubi1_0 $MOUNTPOINT
tar xf $IMG_DIR/rootfs.tar -C $MOUNTPOINT

mkdir -p $MOUNTPOINT/data

cp -a $IMG_DIR/etc/* $MOUNTPOINT/etc
cp -a $IMG_DIR/usr/* $MOUNTPOINT/usr
cp -a $IMG_DIR/data/* $MOUNTPOINT/data
cp -a $IMG_DIR/root/* $MOUNTPOINT/root

sync
# umount $MOUNTPOINT
