#include <gtest/gtest.h>
extern "C" {
  #include "resume_unittest.h"
}

TEST(index_of_first_spaceTest, EmptyString) {
  EXPECT_EQ(0, index_of_first_space("\0"));
}

TEST(index_of_first_spaceTest, NoSpace) {
  EXPECT_EQ(5, index_of_first_space("abcde"));
  EXPECT_EQ(10, index_of_first_space("iamhungryy"));
}

TEST(index_of_first_spaceTest, NormalCase) {
  EXPECT_EQ(5, index_of_first_space("fivey letter word first"));
  EXPECT_EQ(8, index_of_first_space("actually the first word has 8 letters"));
  EXPECT_EQ(3, index_of_first_space("sam is actually my name"));
}
