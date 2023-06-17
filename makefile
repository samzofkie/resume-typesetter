CFLAGS := -g -Wall -Wextra -Werror -std=gnu11 -Wno-unused-parameter `pkg-config --cflags pangocairo`
CXXFLAGS := $(CXXFLAGS) `pkg-config --cflags gtest_main`
LDLIBS := `pkg-config --libs pangocairo`

all: write_resume test

write_resume: main.o resume.o
	$(CC) $^ $(LDLIBS) -o $@

test: resume_unittest.o resume.o
	$(CXX) $^ $(LDLIBS) `pkg-config --libs gtest_main` -o $@

clean:
	rm -f *.o
	rm -f test write_resume
