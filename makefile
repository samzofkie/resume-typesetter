all:
	gcc -O0 `pkg-config --cflags --libs pangocairo` write_resume.c
