#!/bin/bash
rev=`git describe --tags`

if [ "x${rev}" != "x" ]; then
	echo "#define VER_REV \"${rev}\"" > version.h
else
	echo "#define VER_REV \"UNKNOWN\"" > version.h
fi

