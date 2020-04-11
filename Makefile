CXX = clang++
CFLAGS = -g -Wall --std=c++17

LIBS_EDITOR = `pkg-config --static --libs SDL2 SDL2_image`
#LIBS_GAME = `pkg-config --static --libs glew SDL2 SDL2_image SDL2_mixer`
LIBS_GAME = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lglew32 -lopengl32 -lole32 -loleaut32 -lcomdlg32

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
RESOURCE_DIR = resource

BIN_EDITOR = PinoMaze_editor
BIN_GAME = PinoMaze_game

INCLUDE = -I$(SRC_DIR)/shared/include -Itinyfiledialogs -Iresource_pack/include

resource_pack = resource_pack/bin/resource_pack
resource_load = resource_pack/bin/libresource_load.a

libtfd = tinyfiledialogs/libtfd.a

SOURCES_EDITOR = $(wildcard $(SRC_DIR)/editor/*.cpp $(SRC_DIR)/editor/**/*.cpp)
OBJECTS_EDITOR = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES_EDITOR)))

SOURCES_GAME = $(wildcard $(SRC_DIR)/game/*.cpp $(SRC_DIR)/game/**/*.cpp)
OBJECTS_GAME = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES_GAME)))

SHADERS_GLSL = $(wildcard $(SRC_DIR)/game/**/*.glsl)
SHADERS_OBJS = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(basename $(SHADERS_GLSL)))

SOURCES_SHARED = $(wildcard $(SRC_DIR)/shared/*.cpp $(SRC_DIR)/shared/**/*.cpp)
OBJECTS_SHARED = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES_SHARED)))

LIBS_SHARED = $(resource_load) $(libtfd)

RES_TXT_EDITOR = $(RESOURCE_DIR)/editor.txt
RESOURCES_EDITOR = $(patsubst $(RESOURCE_DIR)/%,$(BIN_DIR)/%.dat,$(basename $(RES_TXT_EDITOR)))

RES_TXT_GAME = $(RESOURCE_DIR)/resource.txt $(RESOURCE_DIR)/music.txt
RESOURCES_GAME = $(patsubst $(RESOURCE_DIR)/%,$(BIN_DIR)/%.dat,$(basename $(RES_TXT_GAME)))

ifeq ($(OS),Windows_NT)
	BIN_EDITOR := $(BIN_EDITOR).exe
	BIN_GAME := $(BIN_GAME).exe
	resource_pack := $(resource_pack).exe

	LIBS_GAME += -lopengl32

	ICON_EDITOR := $(OBJ_DIR)/editor.res
	ICON_GAME := $(OBJ_DIR)/game.res
endif

all: editor game

clean: clean_editor clean_game clean_shared

clean_editor:
	rm -f $(BIN_DIR)/$(BIN_EDITOR) $(RESOURCES_EDITOR) $(ICON_EDITOR) $(OBJECTS_EDITOR) $(OBJECTS_EDITOR:.o=.d)

clean_game:
	rm -f $(BIN_DIR)/$(BIN_GAME) $(RESOURCES_GAME) $(SHADERS_OBJS) $(ICON_GAME) $(OBJECTS_GAME) $(OBJECTS_GAME:.o=.d)

clean_shared:
	rm -f $(OBJECTS_SHARED) $(OBJECTS_SHARED:.o=.d)

$(libtfd):
	$(MAKE) -C tinyfiledialogs

$(resource_load): $(resource_pack)
$(resource_pack):
	$(MAKE) -C resource_pack

$(shell mkdir -p $(BIN_DIR) >/dev/null)

editor: $(BIN_DIR)/$(BIN_EDITOR)
$(BIN_DIR)/$(BIN_EDITOR): $(OBJECTS_EDITOR) $(OBJECTS_SHARED) $(LIBS_SHARED) $(RESOURCES_EDITOR) $(ICON_EDITOR)
	$(CXX) -o $(BIN_DIR)/$(BIN_EDITOR) $(LDFLAGS) $(OBJECTS_EDITOR) $(OBJECTS_SHARED) $(LIBS_SHARED) $(LIBS_EDITOR) $(ICON_EDITOR)

game: $(BIN_DIR)/$(BIN_GAME)
$(BIN_DIR)/$(BIN_GAME): $(OBJECTS_GAME) $(OBJECTS_SHARED) $(LIBS_SHARED) $(SHADERS_OBJS) $(RESOURCES_GAME) $(ICON_GAME)
	$(CXX) -o $(BIN_DIR)/$(BIN_GAME) $(LDFLAGS) $(OBJECTS_GAME) $(OBJECTS_SHARED) $(LIBS_SHARED) $(SHADERS_OBJS) $(LIBS_GAME) $(ICON_GAME)

$(OBJ_DIR)/%.res: $(RESOURCE_DIR)/%.rc
	windres $< -O coff -o $@

$(BIN_DIR)/%.dat: $(RESOURCE_DIR)/%.txt $(resource_pack)
	$(resource_pack) $< $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.glsl
	@mkdir -p $(dir $@)
	$(LD) -r -b binary -o $@ $<

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJ_DIR)/$*.Td

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(OBJ_DIR)/%.d
	@mkdir -p $(dir $@)
	$(CXX) $(DEPFLAGS) $(CFLAGS) -c $(INCLUDE) -o $@ $<
	@mv -f $(OBJ_DIR)/$*.Td $(OBJ_DIR)/$*.d && touch $@

$(OBJ_DIR)/%.d: ;
.PRECIOUS: $(OBJ_DIR)/%.d

include $(wildcard $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.d,$(basename $(SOURCES_EDITOR) $(SOURCES_GAME) $(SOURCES_SHARED))))
