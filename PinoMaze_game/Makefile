CXX = g++
LD = g++
CFLAGS = -g -Wall --std=c++14

LDFLAGS =
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lglew32 -lopengl32

PinoMaze_maze_dir = ../PinoMaze_maze

INCLUDE = -Iinclude -I$(PinoMaze_maze_dir)/include
BIN_DIR = ../bin
OBJ_DIR = obj

PinoMaze_maze = $(BIN_DIR)/PinoMaze_maze.so

MAKE = make

OUT_BIN = PinoMaze_game

resource_pack_dir = ../resource_pack
resource_pack = $(resource_pack_dir)/bin/resource_pack
resource_load = $(resource_pack_dir)/bin/libresource_load.a

ifeq ($(OS),Windows_NT)
	LDFLAGS += -mwindows -lmingw32 -lSDL2main
	OUT_BIN := $(OUT_BIN).exe
	resource_pack := $(resource_pack).exe
	PinoMaze_maze := $(PinoMaze_maze:.so=.dll)
	MAKE := mingw32-make
endif

$(shell mkdir -p $(BIN_DIR) >/dev/null)
$(shell mkdir -p $(OBJ_DIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJ_DIR)/$*.Td

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES))) $(resource_load)

RES_TXT = resource/music.txt resource/resource.txt resource/shaders.txt
RESOURCES = $(patsubst resource/%,$(BIN_DIR)/%.dat,$(basename $(RES_TXT)))

all: $(BIN_DIR)/$(OUT_BIN)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(PinoMaze_maze_dir) clean
	$(MAKE) -C $(resource_pack_dir) clean

$(BIN_DIR)/$(OUT_BIN): $(OBJECTS) $(RESOURCES) $(PinoMaze_maze)
	$(LD) -o $(BIN_DIR)/$(OUT_BIN) $(OBJECTS) $(LDFLAGS) $(LIBS) $(PinoMaze_maze)

$(PinoMaze_maze):
	$(MAKE) -C $(PinoMaze_maze_dir)

$(resource_load): $(resource_pack)
$(resource_pack):
	$(MAKE) -C $(resource_pack_dir)

$(OBJ_DIR)/%.o : src/%.cpp
$(OBJ_DIR)/%.o : src/%.cpp $(OBJ_DIR)/%.d
	$(CXX) $(DEPFLAGS) $(CFLAGS) -c $(INCLUDE) -o $@ $<
	@mv -f $(OBJ_DIR)/$*.Td $(OBJ_DIR)/$*.d

$(BIN_DIR)/%.dat: resource/%.txt
	$(resource_pack) $< $@

$(OBJ_DIR)/%.d: ;
.PRECIOUS: $(OBJ_DIR)/%.d

-include $(patsubst src/%,$(OBJ_DIR)/%.d,$(basename $(SOURCES)))
