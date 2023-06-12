#include <gtest/gtest.h>

extern "C" {
  #include "resume_unittest.h"
}


TEST(index_of_first_spaceTest, EmptyString) {
  EXPECT_EQ(0, index_of_first_space(""));
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


class length_longest_str_that_fitsTest : public ::testing::Test {
  protected:
    void SetUp() override {
      init_cairo();
    }

    void TearDown() override {
      cleanup_cairo();
    }
};

TEST_F(length_longest_str_that_fitsTest, EmptyString) {
  EXPECT_EQ(0, length_longest_str_that_fits("", 100));
}

TEST_F(length_longest_str_that_fitsTest, ShouldFit) {
  EXPECT_EQ(8, length_longest_str_that_fits("whats up", doc_width));
  EXPECT_EQ(38, length_longest_str_that_fits("hey there delilah whats it like in nyc", doc_width));
}

TEST_F(length_longest_str_that_fitsTest, TooLong) {
  EXPECT_EQ(103, length_longest_str_that_fits("Counterintelligence-inverted covert operative strategies performing suboptimal reconaisance relative to penetration of adversary resource units, combined with locally sourced cointelligence platforms, create oportunities for leveragable operational strategies.", doc_width));
}
