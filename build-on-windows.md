# Building Hands-Free Sound Jam On Windows

This document provides a comprehensive guide to building MSHFSJ on Windows.


## Git and Msys2

It is strongly advised to use the git-bash utility on Windows, however 
anying capable git utility should work.

EyeJam is not built using Visual Studio. Instead it uses 
[msys2](http://www.msys2.org), a Cygwin-like POSIX compatibility layer with
MinGW-w64 support. Download the 64-bit version and follow the instructions
on the website to get msys2 initially set up. **MSYS2 64-bit installs both
32-bit and 64-bit options. When you launch 64-bit, you will need to 
explicitly launch the 64-bit MSYS.**

## Installing minimal build environment

With MSYS2 64-bit installed and configured, it is now time to set up the minimal
build environment. 

First, install gcc and make.

	pacman -S gcc make

Next, install the mingw-w64-x64 compilation toolchain. Use the "default" option
and install everything. 

	pacman -S mingw-w64-x86_64-toolchain

## Soundpipe

Instead of using the built-in Soundpipe library, the upstream version from
git is used. 

Open up git-bash and clone the github repo, and checkout the "dev" branch:

	git clone git://github.com/paulbatchelor/soundpipe.git
	git checkout dev

In the msys2 shell, install libsndfile:

	pacman -S mingw-w64-x86_64-libsndfile


Go into the newly cloned soundpipe project in msys2, and run 
	
	make config.mk

Then add the following lines to the end of the file config.mk

	echo "CFLAGS += -I/mingw64/include" >> config.mk
	echo "EXT=x86_64-w64-mingw32" >> config.mk
	echo "CC=$(EXT)-gcc" >> config.mk

Compile soundpipe with:

	make

Then install it with:

	make install

NOTE: sudo is not needed! 

## EyeJam

EyeJam can finally be compiled! If the Tobii EyeX drivers have not been 
installed. The software can be downloaded 
[here](http://tobiigaming.com/getstarted/?utm_source=developer.tobii.com) 


Install glew and glfw, both the graphics libraries needed for EyeJam:

	pacman -S mingw-w64-x86_64-glew mingw-w64-x86_64-glfw

Rsync is also used in the build script:

	pacman -S rsync


To build for Windows normally, run:

	make windows

To build for high-resolution displays like the Microsft Surface, run

	make windows-highres

Before the application can be run, assets and dlls must be transferred:

	make transfer

Now MSHFSJ can be run with:

	./MicrosoftHandsFreeSoundJam
