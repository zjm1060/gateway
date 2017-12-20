#!/bin/sh

filepath=$(cd "$(dirname "$0")"; pwd)

#mkdir -p $filepath/../export

#cp -a /data/* $filepath/../export

dtctl -S config.password 990000

cp -a $filepath/goldfish /data/goldfish

chmod +x /data/goldfish

sync

rm -fR /data/firmware/*

rm -fR /data/update/*

reboot