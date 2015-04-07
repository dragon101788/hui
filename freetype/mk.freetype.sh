#!/bin/sh
cd freetype/
echo !!!!!!!!!${TOPDIR}!!!!!!!!!
make distclean
./configure CC=$(CROSS_COMPILE)gcc --host=arm-linux --prefix=$PWD/_install LIBS=-L${TOPDIR}/lib/  LDFLAGS=-L${TOPDIR}/lib/

make
make install
cp _install/libfreetype.a ${TOPDIR}/lib/
make distclean



#./configure CC=arm-linux-gcc --host=arm-linux --prefix=$PWD/_install  LDFLAGS=-L/home/tt/works/hui/hui/lib LDFLAGS=-L/usr/local/arm_linux_4.2/arm-none-linux-gnueabi/lib


