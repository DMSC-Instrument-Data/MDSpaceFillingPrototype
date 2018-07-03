#include <gtest/gtest.h>

#include "MDEvent.h"
#include "Types.h"

TEST(MDEventTest, test_float_coord_round_trip_4_64bit) {
  using Event = MDEvent<4, uint16_t, uint64_t>;

  MDSpaceBounds<4> space;
  // clang-format off
  space <<
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f;
  // clang-format on

  MDCoordinate<4> coord;
  coord << 5.0f, 6.0f, 7.0f, 3.0f;

  Event e(coord, space);

  const auto retrievedCoord = e.coordinates(space);

  EXPECT_NEAR(5.0f, retrievedCoord[0], 1e-4f);
  EXPECT_NEAR(6.0f, retrievedCoord[1], 1e-4f);
  EXPECT_NEAR(7.0f, retrievedCoord[2], 1e-4f);
  EXPECT_NEAR(3.0f, retrievedCoord[3], 1e-4f);
}
