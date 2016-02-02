libs= ./lib/
compiler= g++
flags= -std=c++11 -Wall -m64 -02 -stdlib=libc++ -fexceptions -fvisibility=hidden -Wl,--build-id=sha1
includes= $(pkg-config --cflags --libs opencv)


build:
	$(compiler) \
	$(includes) \
	$(flags)

.PHONY 