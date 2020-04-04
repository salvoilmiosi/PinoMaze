all: maze editor game

clean:
	make -f Makefile.maze clean
	make -f Makefile.editor clean
	make -f Makefile.game clean
	rm -rf bin

maze:
	make -f Makefile.maze
	
editor:
	make -f Makefile.editor

game:
	make -f Makefile.game

.PHONY: clean game editor
