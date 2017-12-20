#!/bin/sh

filepath=$(cd "$(dirname "$0")"; pwd)

sh $filepath/install.sh $filepath/package

reboot