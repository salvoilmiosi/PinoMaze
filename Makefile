all: maze editor game

clean:
	$(MAKE) -f Makefile.maze clean
	$(MAKE) -f Makefile.editor clean
	$(MAKE) -f Makefile.game clean
	rm -rf bin

maze:
	$(MAKE) -f Makefile.maze
	
editor:
	$(MAKE) -f Makefile.editor

game:
	$(MAKE) -f Makefile.game

.PHONY: clean game editor
