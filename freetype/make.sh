#!/bin/sh
./configure CC=arm-linux-gcc --host=arm-linux --prefix=$PWD/_install  LDFLAGS=-L/home/tt/works/hui/hui/lib LDFLAGS=-L/usr/local/arm_linux_4.2/arm-none-linux-gnueabi/lib

make && make install
