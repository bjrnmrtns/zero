all: RenderPipeline gamearchitecture glfwtest fileio deferredconstruction perfectforwarding initstruct webserver

gamearchitecture: gamearchitecture.cpp
	g++ -std=c++0x -Wall -Wextra -pedantic -o gamearchitecture gamearchitecture.cpp 
RenderPipeline: RenderPipeline.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o RenderPipeline RenderPipeline.cpp external/lib/libglfw.a external/lib64/libGLEW.a external/lib/libIL.a -lpng -Iexternal/include -lX11 -lGL
glfwtest: glfwtest.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o glfwtest glfwtest.cpp external/lib/libglfw.a external/lib64/libGLEW.a external/lib/libIL.a -lpng -Iexternal/include -lX11 -lGL
fileio: fileio.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o fileio fileio.cpp -Iexternal/include
deferredconstruction: deferredconstruction.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o deferredconstruction deferredconstruction.cpp -Iexternal/include
perfectforwarding: perfectforwarding.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o perfectforwarding perfectforwarding.cpp -Iexternal/include
initstruct: initstruct.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o initstruct initstruct.cpp -Iexternal/include
webserver: webserver.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o webserver webserver.cpp -lpthread -Iexternal/include

