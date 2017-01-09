CC = g++
AR = ar
CFLAGS = -g -Wall --std=c++11
INCLUDE = include

PACK_BIN = bin/resource_pack
UNPACK_BIN = bin/resource_unpack
LOAD_BIN = bin/libresource_load.a

$(shell mkdir -p bin >/dev/null)
$(shell mkdir -p obj >/dev/null)

all: $(PACK_BIN) $(UNPACK_BIN) $(LOAD_BIN)

clean:
	rm -rf bin
	rm -rf obj

$(PACK_BIN): src/resource_pack.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $<

$(UNPACK_BIN): src/resource_unpack.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $<

$(LOAD_BIN): obj/resource_load.o
	$(AR) rcs $@ $<

obj/resource_load.o: src/resource_load.cpp include/resource_load.h
	$(CC) -c $(CFLAGS) -I$(INCLUDE) -o $@ src/resource_load.cpp
