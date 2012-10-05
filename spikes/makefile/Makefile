ARCH=x86
COMPILER=g++
MODE=release
GAME=rpg

CONFIG=$(ARCH)/$(COMPILER)/$(MODE)
GAME_FILES=$(wildcard games/$(GAME)/*)
GAME_CPP=$(filter %.cpp, %.o $(GAME_FILES))
GAME_OBJECTS=$(patsubst games/$(GAME)/%.cpp,bin/$(CONFIG)/games/$(GAME)/%.o,$(GAME_CPP))
GAME_LIB=bin/$(CONFIG)/games/$(GAME)/lib$(GAME).a
ENGINE_FILES=$(wildcard engine/*)
ENGINE_CPP=$(filter %.cpp, %.o $(ENGINE_FILES))
ENGINE_OBJECTS=$(patsubst engine/%.cpp,bin/$(CONFIG)/engine/%.o,$(ENGINE_CPP))
ENGINE_LIB=bin/$(CONFIG)/engine/libengine.a

all: current bin/$(CONFIG)/games/$(GAME)/$(GAME) Makefile
	@echo Done making $(GAME).

current: bin/$(CONFIG)/games/$(GAME)/$(GAME)
	$(shell ln -n -f -s bin/$(CONFIG)/games/$(GAME) bin/$(GAME))


bin/$(CONFIG)/games/$(GAME)/$(GAME): $(GAME_LIB) $(ENGINE_LIB)
	@g++ -o $@ $^ 
	@echo "Linking $@"

$(GAME_LIB): $(GAME_OBJECTS)
	@ar rcs $@ $^

$(ENGINE_LIB): $(ENGINE_OBJECTS)
	@ar rcs $@ $^

bin/$(CONFIG)/engine/%.o: engine/%.cpp bin/$(CONFIG)/engine/%.d
	@mkdir -p `dirname $@`
	@g++ -std=c++0x -I./engine -c -o $@ $<
	@echo "Compiling $@"

bin/$(CONFIG)/games/$(GAME)/%.o: games/$(GAME)/%.cpp bin/$(CONFIG)/games/$(GAME)/%.d
	@mkdir -p `dirname $@`
	@g++ -std=c++0x -I./engine -I./games/$(GAME) -c -o $@ $<
	@echo "Compiling $@"

bin/$(CONFIG)/games/$(GAME)/%.d: games/$(GAME)/%.cpp 
	@mkdir -p `dirname $@`
	@g++ -I./engine -I./games/$(GAME) -E -MD -M -MM $< -o $@

bin/$(CONFIG)/engine/%.d: engine/%.cpp 
	@mkdir -p `dirname $@`
	@g++ -I./engine -E -MD -M -MM $< -o $@

clean:
	@rm -rf bin
