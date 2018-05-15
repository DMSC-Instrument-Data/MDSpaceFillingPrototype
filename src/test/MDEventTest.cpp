#include <gtest/gtest.h>

#include "MDEvent.h"

TEST(MDEventTest, Create) {
  float coords[] = {3.0f, 4.0f, 5.0f, 6.0f};
  MDEvent<4, uint64_t> event(coords);
  EXPECT_EQ(3.0f, event.coord(0));
  EXPECT_EQ(4.0f, event.coord(1));
  EXPECT_EQ(5.0f, event.coord(2));
  EXPECT_EQ(6.0f, event.coord(3));
}
