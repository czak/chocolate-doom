#!/bin/bash
INSTALL_PREFIX=$PWD/out

cd ../
./autogen.sh --host=arm-webos-linux-gnueabi --without-libsamplerate --prefix=$INSTALL_PREFIX --with-bashcompletiondir=/dev/null
make install