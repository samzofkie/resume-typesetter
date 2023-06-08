CFLAGS = -g -Wall -Wextra -Werror -std=gnu11

write_resume: main.o resume.o
	g++ main.o resume.o `pkg-config --libs pangocairo` -o write_resume
resume.o: resume.c
	g++ `pkg-config --cflags pangocairo` -c resume.c
main.o: main.c
	g++ `pkg-config --cflags pangocairo` -c main.c

clean:
	rm *.o
	rm test write_resume
