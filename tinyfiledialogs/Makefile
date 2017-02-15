CC = gcc
AR = ar
CFLAGS = -g -Wall

OUT = libtfd.a
OBJ = tinyfiledialogs.o

all: $(OUT)

clean:
	rm -f $(OBJ) $(OUT)

$(OUT): $(OBJ)
	$(AR) rcs $@ $<

$(OBJ): tinyfiledialogs.c tinyfiledialogs.h
	$(CC) -c $(CFLAGS) -o $@ $<