#!/bin/sh
cd ${TOPDIR}/libsrc/freetype/
echo !!!!!!!!!${TOPDIR}!!!!!!!!!
make distclean

./configure --host=arm-linux --prefix=${TOPDIR}  LDFLAGS=-L/usr/local/arm_linux_4.2/arm-none-linux-gnueabi/lib
#./configure CC=arm-linux-gcc --host=arm-linux --prefix=${TOPDIR}  LDFLAGS=-L/home/tt/works/hui/hui/lib LDFLAGS=-L/usr/local/arm_linux_4.2/arm-none-linux-gnueabi/lib

make
make install
make distclean




