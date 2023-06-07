all:
	gcc -O0 `pkg-config --cflags --libs pangocairo` resume.c
