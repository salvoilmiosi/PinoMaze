CXX = g++
AR = ar
CFLAGS = -g -Wall --std=c++14

tfd = ../tinyfiledialogs

INCLUDE = -Iinclude -I$(tfd)
BIN_DIR = lib
OBJ_DIR = obj

MAKE = make

OUT_BIN = libmaze.a

$(shell mkdir -p $(BIN_DIR) >/dev/null)
$(shell mkdir -p $(OBJ_DIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJ_DIR)/$*.Td

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%,$(OBJ_DIR)/%.o,$(basename $(SOURCES))) $(resource_load)

all: $(BIN_DIR)/$(OUT_BIN)

clean:
	rm -f $(BIN_DIR)/$(OUT_BIN)
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(tfd) clean

$(BIN_DIR)/$(OUT_BIN): $(OBJECTS) $(tfd)/libtfd.a
	$(AR) rvs $(BIN_DIR)/$(OUT_BIN) $(OBJECTS) $(tfd)/tinyfiledialogs.o

$(tfd)/libtfd.a:
	$(MAKE) -C $(tfd)

$(OBJ_DIR)/%.o : src/%.cpp
$(OBJ_DIR)/%.o : src/%.cpp $(OBJ_DIR)/%.d
	$(CXX) $(DEPFLAGS) $(CFLAGS) -c $(INCLUDE) -o $@ $<
	@mv -f $(OBJ_DIR)/$*.Td $(OBJ_DIR)/$*.d

$(OBJ_DIR)/%.d: ;
.PRECIOUS: $(OBJ_DIR)/%.d

-include $(patsubst src/%,$(OBJ_DIR)/%.d,$(basename $(SOURCES)))
