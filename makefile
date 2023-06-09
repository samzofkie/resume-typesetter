CFLAGS = -g -Wall -Wextra -Werror -std=gnu11

all: write_resume test

write_resume: main.o resume.o
	gcc main.o resume.o `pkg-config --libs pangocairo` -o write_resume
resume.o: resume.c
	gcc `pkg-config --cflags pangocairo` -c resume.c
main.o: main.c
	gcc `pkg-config --cflags pangocairo` -c main.c

test: resume_unittest.o resume.o
	g++ `pkg-config --libs gtest_main pangocairo` -o test resume_unittest.o resume.o
resume_unittest.o: resume_unittest.cc
	g++ `pkg-config --cflags gtest_main pangocairo` -c resume_unittest.cc

clean:
	rm *.o
	rm test write_resume
