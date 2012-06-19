all: zero

zero.o: zero.cpp md5.cpp capture.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -c zero.cpp  -I../external/include 

zero: zero.o
	g++ -ggdb -Wl,--rpath,../external/lib   -o zero zero.o ../external/lib/libglfw.a ../external/lib64/libGLEW.a ../external/lib/libIL.a ../external/lib/libvpx.a -L../external/lib -lRocketCore -lRocketDebugger -lpng -lX11 -lGL


