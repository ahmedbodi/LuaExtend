#!/bin/bash

WorkDir=`dirname "$0"`
cd "$WorkDir"

if [ ! -d ../zlib_config ]
then
 mkdir ../zlib_config
fi

if [ ! -f ../zlib_config/zlib_config.h ]
then
 cp zlib_config.h.in ../zlib_config/zlib_config.h
fi
