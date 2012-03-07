all: zero

zero: zero.cpp
	clang++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o zero zero.cpp external/lib/libglfw.a external/lib64/libGLEW.a external/lib/libIL.a -lpng -Iexternal/include -lX11 -lGL
