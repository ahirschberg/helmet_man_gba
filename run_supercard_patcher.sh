#!/usr/bin/env bash

sc_dir="/home/alex/.wine/drive_c/Program Files (x86)/SC/"
sc_exe="SuperCardsd.exe"

if [ ! `which wine` ]; then
    echo "This program requires wine to run."
    exit 1
fi

pushd "$sc_dir"
if [ ! -x "$sc_exe" ]; then
    echo "$sc_exe is not executable or does not exist. Exiting."
    exit 1
fi

wine "$sc_exe" &
popd
