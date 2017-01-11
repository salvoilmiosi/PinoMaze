CXX = g++
AR = ar
CFLAGS = -g -Wall --std=c++14

INCLUDE = include
BIN_DIR = lib
OBJ_DIR = obj

MAKE = make

OUT_BIN = libmaze.a

ifeq ($(OS),Windows_NT)
	MAKE := mingw32-make
endif

$(shell mkdir -p $(BIN_DIR) >/dev/null)
$(shell mkdir -p $(OBJ_DIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJ_DIR)/$*.Td

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES))) $(resource_load)

all: $(BIN_DIR)/$(OUT_BIN)

clean:
	rm -f $(BIN_DIR)/$(OUT_BIN)
	rm -rf $(OBJ_DIR)

$(BIN_DIR)/$(OUT_BIN): $(OBJECTS)
	$(AR) rvs $(BIN_DIR)/$(OUT_BIN) $(OBJECTS)

$(OBJ_DIR)/%.o : src/%.cpp
$(OBJ_DIR)/%.o : src/%.cpp $(OBJ_DIR)/%.d
	$(CXX) $(DEPFLAGS) $(CFLAGS) -c -I$(INCLUDE) -o $@ $<
	@mv -f $(OBJ_DIR)/$*.Td $(OBJ_DIR)/$*.d

$(OBJ_DIR)/%.d: ;
.PRECIOUS: $(OBJ_DIR)/%.d

-include $(patsubst src/%,$(OBJ_DIR)/%.d,$(basename $(SOURCES)))
