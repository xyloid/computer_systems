#!/bin/sh
export LD_PRELOAD=$PWD/libhack.so 
export LD_LIBRARY_PATH=$PWD
echo $LD_PRELOAD
./lotto
unset LD_PRELOAD
