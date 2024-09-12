cam:
	gcc camv4l.c filters.c cam.c -o cam -lSDL2 -lm

dep:
	sudo apt-get install libsdl2-dev libsdl2-2.0-0 v4l-utils
