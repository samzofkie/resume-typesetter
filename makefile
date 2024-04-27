SRC_DIR := src
OBJ_DIR := obj

# This will find all the files ending in .cc in the $(SRC_DIR) directory
SRC_FILES := $(wildcard $(SRC_DIR)/*.cc)
# This uses 'string substitution' ($(text:pattern=replacement)) to generate 
# a list of object files 
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

CXX := clang++
CPPFLAGS := `pkg-config --cflags pangocairo` -Iinclude
CXXFLAGS := -Wall -Wextra -Werror
LDLIBS := `pkg-config --libs pangocairo`

.PHONY: all

all: write-resume

write-resume: write-resume.cc $(OBJ_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDLIBS) $(OBJ_FILES) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)