CXX = clang++
LD = clang++
AR = ar
CFLAGS = -g -Wall --std=c++17

LIBS_EDITOR = `pkg-config --static --libs SDL2 SDL2_image`
LIBS_GAME = `pkg-config --static --libs glew SDL2 SDL2_image SDL2_mixer`

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
RESOURCE_DIR = resource

BIN_EDITOR = PinoMaze_editor
BIN_GAME = PinoMaze_game
PinoMaze_maze = $(OBJ_DIR)/maze/libmaze.a

INCLUDE = -I$(SRC_DIR)/maze/include -Itinyfiledialogs

resource_pack = resource_pack/bin/resource_pack
resource_load = resource_pack/bin/libresource_load.a

tinyfd = tinyfiledialogs/libtfd.a

ifeq ($(OS),Windows_NT)
	BIN_EDITOR := $(BIN_EDITOR).exe
	BIN_GAME := $(BIN_GAME).exe
	resource_pack := $(resource_pack).exe

	LIBS_GAME += -lopengl32

	ICON_EDITOR := $(OBJ_DIR)/icon_editor.res
	ICON_GAME := $(OBJ_DIR)/icon_game.res
endif

$(shell mkdir -p $(BIN_DIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJ_DIR)/$*.Td

SOURCES_EDITOR = $(wildcard $(SRC_DIR)/editor/*.cpp $(SRC_DIR)/editor/**/*.cpp)
OBJECTS_EDITOR = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES_EDITOR))) $(resource_load)

SOURCES_GAME = $(wildcard $(SRC_DIR)/game/*.cpp $(SRC_DIR)/game/**/*.cpp)
OBJECTS_GAME = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES_GAME))) $(resource_load)

SOURCES_MAZE = $(wildcard $(SRC_DIR)/maze/*.cpp $(SRC_DIR)/maze/**/*.cpp)
OBJECTS_MAZE = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES_MAZE))) $(resource_load)

RES_TXT = $(wildcard $(RESOURCE_DIR)/*.txt)
RESOURCES = $(patsubst $(RESOURCE_DIR)/%,$(BIN_DIR)/%.dat,$(basename $(RES_TXT)))

all: maze editor game

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

editor: $(BIN_DIR)/$(BIN_EDITOR)
$(BIN_DIR)/$(BIN_EDITOR): $(OBJECTS_EDITOR) $(RESOURCES) $(PinoMaze_maze) $(tinyfd) $(ICON_EDITOR)
	@mkdir -p $(BIN_DIR)
	$(LD) -o $(BIN_DIR)/$(BIN_EDITOR) $(OBJECTS_EDITOR) $(PinoMaze_maze) $(tinyfd) $(LIBS_EDITOR) $(ICON_EDITOR)

$(OBJ_DIR)/icon_editor.res: $(RESOURCE_DIR)/editor.rc
	windres $< -O coff -o $@

game: $(BIN_DIR)/$(BIN_GAME)
$(BIN_DIR)/$(BIN_GAME): $(OBJECTS_GAME) $(RESOURCES) $(PinoMaze_maze) $(tinyfd) $(ICON_GAME)
	$(LD) -o $(BIN_DIR)/$(BIN_GAME) $(OBJECTS_GAME) $(LDFLAGS) $(PinoMaze_maze) $(tinyfd) $(LIBS_GAME) $(ICON_GAME)

$(OBJ_DIR)/icon_game.res: $(RESOURCE_DIR)/game.rc
	windres $< -O coff -o $@

maze: $(PinoMaze_maze)
$(PinoMaze_maze): $(OBJECTS_MAZE)
	$(AR) rvs $@ $(OBJECTS_MAZE)

$(tinyfd):
	$(MAKE) -C tinyfiledialogs

$(resource_load): $(resource_pack)
$(resource_pack):
	$(MAKE) -C $(resource_pack_dir)

$(BIN_DIR)/%.dat: $(RESOURCE_DIR)/%.txt $(resource_pack)
	$(resource_pack) $< $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(OBJ_DIR)/%.d
	@mkdir -p $(dir $@)
	$(CXX) $(DEPFLAGS) $(CFLAGS) -c $(INCLUDE) -o $@ $<
	@mv -f $(OBJ_DIR)/$*.Td $(OBJ_DIR)/$*.d

$(OBJ_DIR)/%.d: ;
.PRECIOUS: $(OBJ_DIR)/%.d

-include $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.d,$(basename $(SOURCES)))
