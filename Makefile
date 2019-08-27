MAKE = make

ifeq ($(OS),Windows_NT)
	MAKE := mingw32-make
endif

all: game editor

clean:
	$(MAKE) -C PinoMaze_game clean
	$(MAKE) -C PinoMaze_editor clean
	rm -rf bin

game:
	$(MAKE) -C PinoMaze_game
	
editor:
	$(MAKE) -C PinoMaze_editor

.PHONY: clean game editor
