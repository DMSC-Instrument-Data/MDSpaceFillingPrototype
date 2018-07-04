#include <gtest/gtest.h>

#include "CoordinateConversion.h"
#include "Types.h"

TEST(CoordinateConversionTest, CalculateRequiredCoordinateIntegerWidth) {
  MDSpaceBounds<2> bounds;
  // clang-format off
  bounds <<
    1.0f, 8.0f,
    5.0f, 6.0f;
  // clang-format on

  MDSpaceSteps<2> steps;
  steps << 0.02f, 0.01f;

  size_t result = CalculateRequiredCoordinateIntegerWidth<2>(bounds, steps);

  EXPECT_EQ(9, result);
}

TEST(CoordinateConversionTest, ExpandBounds) {
  MDSpaceBounds<4> bounds;
  // clang-format off
  bounds <<
    -1.0f, 2.0f,
    -0.2f, 10.0f,
    3.0f, 7.0f,
    5.0f, 15.0f;
  // clang-format on

  ExpandBounds<4>(bounds);

  EXPECT_LT(bounds.col(0)[0], -1.0f);
  EXPECT_LT(bounds.col(0)[1], -0.2f);
  EXPECT_LT(bounds.col(0)[2], 3.0f);
  EXPECT_LT(bounds.col(0)[3], 5.0f);

  EXPECT_GT(bounds.col(1)[0], 2.0f);
  EXPECT_GT(bounds.col(1)[1], 10.0f);
  EXPECT_GT(bounds.col(1)[2], 7.0f);
  EXPECT_GT(bounds.col(1)[3], 15.0f);
}

TEST(CoordinateConversionTest, CheckCoordinatesInMDSpace) {
  MDSpaceBounds<2> bounds;
  // clang-format off
  bounds <<
    1.0f, 8.0f,
    5.0f, 6.0f;
  // clang-format on

  {
    MDCoordinate<2> coord;
    coord << 5.0f, 5.5f;

    EXPECT_TRUE(CheckCoordinatesInMDSpace(bounds, coord));
  }

  {
    MDCoordinate<2> coord;
    coord << 0.5f, 5.5f;

    EXPECT_FALSE(CheckCoordinatesInMDSpace(bounds, coord));
  }

  {
    MDCoordinate<2> coord;
    coord << 5.0f, 7.0f;

    EXPECT_FALSE(CheckCoordinatesInMDSpace(bounds, coord));
  }
}

TEST(CoordinateConversionTest, ConvertFloatCoordsTo8BitInteger) {
  MDSpaceBounds<4> bounds;
  // clang-format off
  bounds <<
    0.0f, 2.0f,
    0.0f, 10.0f,
    3.0f, 7.0f,
    5.0f, 15.0f;
  // clang-format on

  MDCoordinate<4> floatCoord;
  floatCoord << 1.5f, 10.0f, 5.0f, 5.0f;

  const auto result =
      ConvertCoordinatesToIntegerRange<4, uint8_t>(bounds, floatCoord);

  EXPECT_EQ(191, result[0]);
  EXPECT_EQ(255, result[1]);
  EXPECT_EQ(127, result[2]);
  EXPECT_EQ(0, result[3]);
}

TEST(CoordinateConversionTest, ConvertFloatCoordsFrom8BitInteger) {
  MDSpaceBounds<4> bounds;
  // clang-format off
  bounds <<
    0.0f, 2.0f,
    0.0f, 10.0f,
    3.0f, 7.0f,
    5.0f, 15.0f;
  // clang-format on

  Eigen::Array<uint8_t, 4, 1> intCoord;
  intCoord << 191, 255, 127, 0;

  const auto result =
      ConvertCoordinatesFromIntegerRange<4, uint8_t>(bounds, intCoord);

  EXPECT_NEAR(1.5f, result[0], 0.1f);
  EXPECT_FLOAT_EQ(10.0f, result[1]);
  EXPECT_NEAR(5.0f, result[2], 0.2f);
  EXPECT_FLOAT_EQ(5.0f, result[3]);
}

TEST(CoordinateConversionTest, ConvertCoordinateToIntegerRange) {
  EXPECT_EQ(191, ConvertCoordinateToIntegerRange<uint8_t>(1.5f, 0.0f, 2.0f));
  EXPECT_EQ(255, ConvertCoordinateToIntegerRange<uint8_t>(10.0f, 0.0f, 10.0f));
  EXPECT_EQ(127, ConvertCoordinateToIntegerRange<uint8_t>(5.0f, 3.0f, 7.0f));
  EXPECT_EQ(0, ConvertCoordinateToIntegerRange<uint8_t>(5.0f, 5.0f, 15.0f));
}

TEST(CoordinateConversionTest, ConvertCoordinateFromIntegerRange) {
  EXPECT_NEAR(1.5f, ConvertCoordinateFromIntegerRange<uint8_t>(191, 0.0f, 2.0f),
              0.1f);
  EXPECT_FLOAT_EQ(10.0f,
                  ConvertCoordinateFromIntegerRange<uint8_t>(255, 0.0f, 10.0f));
  EXPECT_NEAR(5.0f, ConvertCoordinateFromIntegerRange<uint8_t>(127, 3.0f, 7.0f),
              0.2f);
  EXPECT_FLOAT_EQ(5.0f,
                  ConvertCoordinateFromIntegerRange<uint8_t>(0, 5.0f, 15.0f));
}

/* unsigned char, unsigned short, unsigned int, unsigned long */
typedef testing::Types<uint8_t, uint16_t, uint32_t, uint64_t> IntegerTypes;

template <class T>
class CoordinateConversionTemplatedTest : public testing::Test {
protected:
  template <int N>
  void RoundTripTestExtendedRanges(MDSpaceBounds<N> bounds,
                                   const MDCoordinate<N> floatCoord,
                                   const float prec) {
    ExpandBounds<N>(bounds);

    const auto intCoord =
        ConvertCoordinatesToIntegerRange<N, T>(bounds, floatCoord);

    const auto result =
        ConvertCoordinatesFromIntegerRange<N, T>(bounds, intCoord);

    for (int i = 0; i < N; i++) {
      EXPECT_NEAR(floatCoord[i], result[i], prec);
    }
  }
};
TYPED_TEST_CASE(CoordinateConversionTemplatedTest, IntegerTypes);

TYPED_TEST(CoordinateConversionTemplatedTest, RoundTrip4DExtendedRangeTest) {
  MDSpaceBounds<4> bounds;
  // clang-format off
  bounds <<
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f;
  // clang-format on

  MDCoordinate<4> floatCoord;
  floatCoord << 1.5f, 10.0f, 5.0f, 0.0f;

  this->RoundTripTestExtendedRanges(bounds, floatCoord, 0.1f);
}
