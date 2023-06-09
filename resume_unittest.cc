#include <gtest/gtest.h>

extern "C" {
  #include "resume.h"
}

TEST(index_of_next_spaceTest, OutOfBounds) {
  EXPECT_EQ(-1, index_of_next_space("Whats up", -1));
  EXPECT_EQ(-1, index_of_next_space("Whats up", 8));
}
