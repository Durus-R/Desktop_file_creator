#! /bin/bash
mkdir bin_release
cmake -B bin_release
cd bin_release
make 
mkdir AppDir
make DESTDIR=./AppDir install
cd ..
mkdir bin_release/AppDir/lib
mkdir bin_release/AppDir/share
mkdir bin_release/AppDir/share/applications
