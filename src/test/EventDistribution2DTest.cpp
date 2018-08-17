#include <gtest/gtest.h>

#include <algorithm>
#include <limits>

#include <boost/sort/sort.hpp>

#include "TestUtil.h"

#include "MDBox.h"
#include "MDEvent.h"

constexpr size_t ND(2);
using IntT = uint16_t;
using MortonT = uint32_t;
constexpr auto interleaveFunc = interleave<ND, IntT, MortonT>;

using Event = MDEvent<ND, IntT, MortonT>;
using Coord = MDCoordinate<ND>;
using Space = MDSpaceBounds<ND>;
using Box = MDBox<ND, IntT, MortonT>;

/**
 * Generates a sample event distribution to test curve order and box/tree
 * generation.
 *
 * A visual representation of this is available at
 * docs/test_2d_structure_1.jpg.
 *
 * @param events Storage for events
 * @param space MD space bounds
 */
void fill_events_for_pattern_1(std::vector<Event> &events, const Space &space) {
  /* Box 1 (2 events, depth 2, min (-10,-5), max (-5,-2.5)) */
  events.emplace_back(Coord{-9.9499f, -3.8113f}, space, 0.1101);
  events.emplace_back(Coord{-9.1842f, -4.6146f}, space, 0.1102);

  /* Box 2 (8 events, depth 3, min (-5,-5), max (-2.5,-3.75)) */
  events.emplace_back(Coord{-4.3669f, -4.9124f}, space, 0.12101);
  events.emplace_back(Coord{-3.9690f, -4.3919f}, space, 0.12102);
  events.emplace_back(Coord{-3.9690f, -4.1091f}, space, 0.12103);
  events.emplace_back(Coord{-4.0841f, -4.1554f}, space, 0.12104);
  events.emplace_back(Coord{-4.7923f, -4.4069f}, space, 0.12105);
  events.emplace_back(Coord{-2.9605f, -4.8111f}, space, 0.12106);
  events.emplace_back(Coord{-4.0966f, -4.1504f}, space, 0.12107);
  events.emplace_back(Coord{-4.1667f, -4.0290f}, space, 0.12108);

  /* Box 3 (9 events, depth 3, min (-2.5,-5), max (0, -3.75)) */
  events.emplace_back(Coord{-1.8293f, -3.9865f}, space, 0.12201);
  events.emplace_back(Coord{-2.4650f, -4.3806f}, space, 0.12202);
  events.emplace_back(Coord{-2.0546f, -4.6221f}, space, 0.12203);
  events.emplace_back(Coord{-1.2563f, -3.9790f}, space, 0.12204);
  events.emplace_back(Coord{-1.1712f, -4.2417f}, space, 0.12205);
  events.emplace_back(Coord{-1.8218f, -4.2117f}, space, 0.12206);
  events.emplace_back(Coord{-1.3714f, -4.1041f}, space, 0.12207);
  events.emplace_back(Coord{-0.8984f, -4.6709f}, space, 0.12208);
  events.emplace_back(Coord{-1.0010f, -4.5821f}, space, 0.12209);

  /* Box 4 (6 events, depth 3, min (-5,-3.75), max (-2.5,-2.5)) */
  events.emplace_back(Coord{-4.0591f, -3.4697f}, space, 0.12301);
  events.emplace_back(Coord{-4.3869f, -2.6964f}, space, 0.12302);
  events.emplace_back(Coord{-3.2382f, -2.6764f}, space, 0.12303);
  events.emplace_back(Coord{-4.5746f, -2.5038f}, space, 0.12304);
  events.emplace_back(Coord{-3.8163f, -3.7337f}, space, 0.12305);
  events.emplace_back(Coord{-2.6752f, -2.5400f}, space, 0.12306);

  /* Box 5 (3 events, depth 3, min (-2.5,-3.75), max (0,-2.5)) */
  events.emplace_back(Coord{-2.4900f, -3.0668f}, space, 0.12401);
  events.emplace_back(Coord{-0.4329f, -2.9780f}, space, 0.12402);
  events.emplace_back(Coord{-1.6942f, -2.8704f}, space, 0.12403);

  /* Box 6 (3 events, depth 2, min (-10,-2.5), max (-5,0)) */
  events.emplace_back(Coord{-7.0050f, -0.1777f}, space, 0.1301);
  events.emplace_back(Coord{-6.1772f, -1.1512f}, space, 0.1302);
  events.emplace_back(Coord{-8.9179f, -0.0951f}, space, 0.1303);

  /* Box 7 (6 events, depth 2, min (-5,-2.5), max (0,0)) */
  events.emplace_back(Coord{-0.6557f, -1.5866f}, space, 0.1401);
  events.emplace_back(Coord{-1.0661f, -0.6832f}, space, 0.1402);
  events.emplace_back(Coord{-3.6537f, -0.2202f}, space, 0.1403);
  events.emplace_back(Coord{-2.6226f, -0.4680f}, space, 0.1404);
  events.emplace_back(Coord{-1.7918f, -0.4304f}, space, 0.1405);
  events.emplace_back(Coord{-4.6797f, -1.8043f}, space, 0.1406);

  /* Box 8 (7 events, depth 1, min (0,-5), max (10,0)) */
  events.emplace_back(Coord{6.5866f, -2.3624f}, space, 0.201);
  events.emplace_back(Coord{7.9179f, -1.9169f}, space, 0.202);
  events.emplace_back(Coord{8.5686f, -2.8629f}, space, 0.203);
  events.emplace_back(Coord{3.8438f, -4.1892f}, space, 0.204);
  events.emplace_back(Coord{9.3493f, -0.5756f}, space, 0.205);
  events.emplace_back(Coord{1.7417f, -4.5546f}, space, 0.206);
  events.emplace_back(Coord{3.2633f, -0.4104f}, space, 0.207);

  /* Box 9 (7 events, depth 2, min (-10,0), max (-5,2.5)) */
  events.emplace_back(Coord{-9.6897f, 1.2638f}, space, 0.3101);
  events.emplace_back(Coord{-7.1872f, 1.8994f}, space, 0.3102);
  events.emplace_back(Coord{-5.1001f, 2.2022f}, space, 0.3103);
  events.emplace_back(Coord{-8.3433f, 1.8118f}, space, 0.3104);
  events.emplace_back(Coord{-8.3984f, 0.9510f}, space, 0.3105);
  events.emplace_back(Coord{-6.5365f, 0.2252f}, space, 0.3106);
  events.emplace_back(Coord{-8.1832f, 1.7568f}, space, 0.3107);

  /* Box 10 (1 event, depth 2, min (-5,0), max (0,2.5)) */
  events.emplace_back(Coord{-1.0210f, 0.1902f}, space, 0.3201);

  /* Box 11 (3 events, depth 2, min (-10,2.5), max (-5,5)) */
  events.emplace_back(Coord{-6.0511f, 2.8403f}, space, 0.3301);
  events.emplace_back(Coord{-7.3824f, 4.3569f}, space, 0.3302);
  events.emplace_back(Coord{-8.2032f, 4.9650f}, space, 0.3303);

  /* Box 12 (8 events, depth 5, min (-5,2.5), max (-4.375,2.8125)) */
  events.emplace_back(Coord{-4.4910f, 2.5644f}, space, 0.3411101);
  events.emplace_back(Coord{-4.9324f, 2.6017f}, space, 0.3411102);
  events.emplace_back(Coord{-4.6772f, 2.6092f}, space, 0.3411103);
  events.emplace_back(Coord{-4.6359f, 2.5619f}, space, 0.3411104);
  events.emplace_back(Coord{-3.9643f, 2.5875f}, space, 0.3411105);
  events.emplace_back(Coord{-3.8908f, 2.6442f}, space, 0.3411106);
  events.emplace_back(Coord{-4.5154f, 2.6000f}, space, 0.3411107);
  events.emplace_back(Coord{-3.8607f, 2.6567f}, space, 0.3411108);

  /* Box 13 (1 event, depth 5, min (-4.375,2.5), max (-3.75,2.8125)) */
  events.emplace_back(Coord{-4.0143f, 2.5022f}, space, 0.3411201);

  /* Box 14 (0 events, depth 5, min (-5,2.8125), max (-4.375,3.125)) */

  /* Box 15 (0 events, depth 5, min (-4.375,2.8125), max (-3.75,3.125)) */

  /* Box 16 (7 events, depth 4, min (-3.75,2.5), max (-2.5,3.125)) */
  events.emplace_back(Coord{-3.3674f, 3.0662f}, space, 0.341201);
  events.emplace_back(Coord{-3.4059f, 2.7903f}, space, 0.341202);
  events.emplace_back(Coord{-3.1369f, 2.8310f}, space, 0.341203);
  events.emplace_back(Coord{-3.3208f, 2.8519f}, space, 0.341204);
  events.emplace_back(Coord{-3.3949f, 2.9101f}, space, 0.341205);
  events.emplace_back(Coord{-2.6114f, 2.8938f}, space, 0.341206);
  events.emplace_back(Coord{-2.9792f, 2.6326f}, space, 0.341207);

  /* Box 17 (0 events, depth 4, min (-5,3.125), max (-3.75,3.75)) */

  /* Box 18 (0 events, depth 4, min (-3.75,3.125), max (-2.5,3.75)) */

  /* Box 19 (9 events, depth 3, min (-2.5,2.5), max (0,3.75)) */
  events.emplace_back(Coord{-2.4474f, 3.6324f}, space, 0.34201);
  events.emplace_back(Coord{-0.0025f, 2.7227f}, space, 0.34202);
  events.emplace_back(Coord{-1.3839f, 3.5598f}, space, 0.34203);
  events.emplace_back(Coord{-1.8068f, 3.1982f}, space, 0.34204);
  events.emplace_back(Coord{-0.6256f, 3.0105f}, space, 0.34205);
  events.emplace_back(Coord{-1.2487f, 2.8941f}, space, 0.34206);
  events.emplace_back(Coord{-1.0135f, 3.5410f}, space, 0.34207);
  events.emplace_back(Coord{-2.3974f, 3.0393f}, space, 0.34208);
  events.emplace_back(Coord{-1.7217f, 2.7427f}, space, 0.34209);

  /* Box 20 (2 events, depth 3, min (-5,3.75), max (-2.5,5)) */
  events.emplace_back(Coord{-2.8003f, 4.5821f}, space, 0.34301);
  events.emplace_back(Coord{-3.1181f, 4.4019f}, space, 0.34302);

  /* Box 21 (1 events, depth 3, min (-2.5,3.75), max (0,5)) */
  events.emplace_back(Coord{-0.4880f, 3.7888f}, space, 0.34401);

  /* Box 22 (4 events, depth 2, min (0,0), max (5,2.5)) */
  events.emplace_back(Coord{3.1832f, 0.6807f}, space, 0.4101);
  events.emplace_back(Coord{2.1672f, 1.8869f}, space, 0.4102);
  events.emplace_back(Coord{4.3944f, 0.4179f}, space, 0.4103);
  events.emplace_back(Coord{1.7317f, 1.3739f}, space, 0.4104);

  /* Box 23 (3 events, depth 2, min (5,0), max (10,2.5)) */
  events.emplace_back(Coord{7.1371f, 2.2573f}, space, 0.4201);
  events.emplace_back(Coord{9.7097f, 2.0020f}, space, 0.4202);
  events.emplace_back(Coord{8.4635f, 1.8268f}, space, 0.4203);

  /* Box 24 (9 events, depth 2, min (0,2.5), max (5,5)) */
  events.emplace_back(Coord{0.2603f, 2.7057f}, space, 0.4301);
  events.emplace_back(Coord{0.6006f, 3.8293f}, space, 0.4302);
  events.emplace_back(Coord{1.6767f, 4.0886f}, space, 0.4303);
  events.emplace_back(Coord{2.3323f, 3.5000f}, space, 0.4304);
  events.emplace_back(Coord{1.1361f, 3.4765f}, space, 0.4305);
  events.emplace_back(Coord{1.0060f, 3.7017f}, space, 0.4306);
  events.emplace_back(Coord{2.0370f, 4.2152f}, space, 0.4307);
  events.emplace_back(Coord{0.6306f, 2.7146f}, space, 0.4308);
  events.emplace_back(Coord{1.7868f, 4.9084f}, space, 0.4309);

  /* Box 25 (8 events, depth 2, min (5,2.5), max (10,5)) */
  events.emplace_back(Coord{7.1221f, 4.9875f}, space, 0.4401);
  events.emplace_back(Coord{7.3924f, 3.1857f}, space, 0.4402);
  events.emplace_back(Coord{8.4535f, 3.3108f}, space, 0.4403);
  events.emplace_back(Coord{9.3794f, 2.7528f}, space, 0.4404);
  events.emplace_back(Coord{7.6276f, 3.5210f}, space, 0.4405);
  events.emplace_back(Coord{8.0981f, 4.7297f}, space, 0.4406);
  events.emplace_back(Coord{8.6737f, 2.9780f}, space, 0.4407);
  events.emplace_back(Coord{8.8338f, 4.8248f}, space, 0.4408);

  /* Randomly shuffle event list */
  std::random_shuffle(events.begin(), events.end());
}

TEST(EventDistribution2DTest, test_pattern_1) {
  std::vector<Event> events;

  /* Define MD space */
  Space space;
  // clang-format off
  space <<
    -10.0f, 10.0f,
    -5.0f, 5.0f;
  // clang-format on

  /* Fill events with known positions */
  fill_events_for_pattern_1(events, space);

  /* Sanity check */
  EXPECT_EQ(107, events.size());

  /* Sort events */
  boost::sort::block_indirect_sort(events.begin(), events.end());

  /* Create box structure */
  Box root(events.cbegin(), events.cend());
  root.distributeEvents(10, 5);

  EXPECT_EQ(107, root.eventCount());

  /* Expected box structure */
  ExpectedBox expectedRoot{
      107,
      {
          {37,
           {
               {2, {}},
               {26,
                {
                    {8, {}}, {9, {}}, {6, {}}, {3, {}},
                }},
               {3, {}},
               {6, {}},
           }},
          {7, {}},
          {39,
           {
               {7, {}},
               {1, {}},
               {3, {}},
               {28,
                {
                    {16,
                     {
                         {9, {}}, {7, {}}, {0, {}}, {0, {}},
                     }},
                    {9, {}},
                    {2, {}},
                    {1, {}},
                }},
           }},
          {24,
           {
               {4, {}}, {3, {}}, {9, {}}, {8, {}},
           }},
      }};

  /* Validate structure */
  Box::ZCurveIterator curveIt = events.cbegin();
  recursive_box_tree_validation(root, curveIt, expectedRoot);
}
