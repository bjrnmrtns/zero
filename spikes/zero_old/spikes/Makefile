all: deferred typeinfo webserver pnglite mtlparse renderpipeline resourceloading bullet_hw networking objectserialization gamearch

deferred: deferred.cpp
	g++ -std=c++0x -Wall -Wextra -o deferred deferred.cpp ../external/lib/libSDL.a ../external/lib/libGLEW.a -I../external/include -lGL -lGLU -lpthread -ldl
typeinfo: typeinfo.cpp
	g++ -std=c++0x -Wall -Wextra -o typeinfo typeinfo.cpp
webserver: webserver.cpp
	g++ -std=c++0x -Wall -Wextra -lpthread -o webserver webserver.cpp
../pnglite/pnglite.o: ../pnglite/pnglite.c
	gcc -ggdb -Wall -Wextra -o ../pnglite/pnglite.o -c ../pnglite/pnglite.c
pnglite: pnglite.cpp ../pnglite/pnglite.o
	g++ -ggdb -std=c++0x -Wall -Wextra -o pnglite pnglite.cpp ../pnglite/pnglite.o -I../pnglite/ -lz
mtlparse: mtlparse.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -o mtlparse mtlparse.cpp
renderpipeline: renderpipeline.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -o renderpipeline renderpipeline.cpp
containerissue: containerissue.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -o renderpipeline renderpipeline.cpp
resourceloading: resourceloading.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -o resourceloading resourceloading.cpp
bullet_hw: bullet_hw.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -o bullet_hw bullet_hw.cpp -lGL -lGLU -I ../external/include/bullet/ ../external/lib/libBulletDynamics.a ../external/lib/libBulletCollision.a ../external/lib/libLinearMath.a
objectserialization: objectserialization.cpp
	g++ -std=c++0x -Wall -Wextra -o objectserialization objectserialization.cpp
networking: networking.cpp
	g++ -std=c++0x -Wall -Wextra -o networking networking.cpp
gamearch: gamearch.cpp
	g++ -std=c++0x -Wall -Wextra -o gamearch gamearch.cpp
