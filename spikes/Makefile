all: RenderPipeline gamearchitecture fileio deferredconstruction perfectforwarding initstruct webserver md5

gamearchitecture: gamearchitecture.cpp
	g++ -std=c++0x -Wall -Wextra -pedantic -o gamearchitecture gamearchitecture.cpp 
RenderPipeline: RenderPipeline.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o RenderPipeline RenderPipeline.cpp -Iexternal/include
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
md5: md5.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o md5 md5.cpp -I../external/include

