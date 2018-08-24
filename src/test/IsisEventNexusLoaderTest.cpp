#include <gtest/gtest.h>

#include "IsisEventNexusLoader.h"

TEST(IsisEventNexusLoaderTest, load_frames_all) {
  IsisEventNexusLoader loader("test_data/isis_event.nxs",
                              "raw_data_1/detector_1_events");

  EXPECT_EQ(15, loader.totalEventCount());
  EXPECT_EQ(4, loader.frameCount());

  std::vector<TofEvent> events;
  loader.loadFrames(events, std::vector<size_t>{0, 1, 2, 3});

  EXPECT_EQ(15, events.size());

  EXPECT_EQ(1, events[0].id);
  EXPECT_EQ(1, events[1].id);
  EXPECT_EQ(2, events[2].id);
  EXPECT_EQ(2, events[3].id);
  EXPECT_EQ(2, events[4].id);
  EXPECT_EQ(3, events[5].id);
  EXPECT_EQ(3, events[6].id);
  EXPECT_EQ(2, events[7].id);
  EXPECT_EQ(2, events[8].id);
  EXPECT_EQ(2, events[9].id);
  EXPECT_EQ(1, events[10].id);
  EXPECT_EQ(1, events[11].id);
  EXPECT_EQ(1, events[12].id);
  EXPECT_EQ(1, events[13].id);
  EXPECT_EQ(2, events[14].id);

  EXPECT_FLOAT_EQ(0.01f, events[0].tof);
  EXPECT_FLOAT_EQ(0.011f, events[1].tof);
  EXPECT_FLOAT_EQ(0.012f, events[2].tof);
  EXPECT_FLOAT_EQ(0.013f, events[3].tof);
  EXPECT_FLOAT_EQ(0.014f, events[4].tof);
  EXPECT_FLOAT_EQ(0.02f, events[5].tof);
  EXPECT_FLOAT_EQ(0.021f, events[6].tof);
  EXPECT_FLOAT_EQ(0.022f, events[7].tof);
  EXPECT_FLOAT_EQ(0.025f, events[8].tof);
  EXPECT_FLOAT_EQ(0.029f, events[9].tof);
  EXPECT_FLOAT_EQ(0.03f, events[10].tof);
  EXPECT_FLOAT_EQ(0.031f, events[11].tof);
  EXPECT_FLOAT_EQ(0.034f, events[12].tof);
  EXPECT_FLOAT_EQ(0.04f, events[13].tof);
  EXPECT_FLOAT_EQ(0.05f, events[14].tof);
}

TEST(IsisEventNexusLoaderTest, load_frame_0) {
  IsisEventNexusLoader loader("test_data/isis_event.nxs",
                              "raw_data_1/detector_1_events");

  EXPECT_EQ(15, loader.totalEventCount());
  EXPECT_EQ(4, loader.frameCount());

  std::vector<TofEvent> events;
  loader.loadFrames(events, std::vector<size_t>{0});

  EXPECT_EQ(4, events.size());

  EXPECT_EQ(1, events[0].id);
  EXPECT_EQ(1, events[1].id);
  EXPECT_EQ(2, events[2].id);
  EXPECT_EQ(2, events[3].id);

  EXPECT_FLOAT_EQ(0.01f, events[0].tof);
  EXPECT_FLOAT_EQ(0.011f, events[1].tof);
  EXPECT_FLOAT_EQ(0.012f, events[2].tof);
  EXPECT_FLOAT_EQ(0.013f, events[3].tof);
}

TEST(IsisEventNexusLoaderTest, load_frame_3) {
  IsisEventNexusLoader loader("test_data/isis_event.nxs",
                              "raw_data_1/detector_1_events");

  EXPECT_EQ(15, loader.totalEventCount());
  EXPECT_EQ(4, loader.frameCount());

  std::vector<TofEvent> events;
  loader.loadFrames(events, std::vector<size_t>{3});

  EXPECT_EQ(5, events.size());

  EXPECT_EQ(1, events[0].id);
  EXPECT_EQ(1, events[1].id);
  EXPECT_EQ(1, events[2].id);
  EXPECT_EQ(1, events[3].id);
  EXPECT_EQ(2, events[4].id);

  EXPECT_FLOAT_EQ(0.03f, events[0].tof);
  EXPECT_FLOAT_EQ(0.031f, events[1].tof);
  EXPECT_FLOAT_EQ(0.034f, events[2].tof);
  EXPECT_FLOAT_EQ(0.04f, events[3].tof);
  EXPECT_FLOAT_EQ(0.05f, events[4].tof);
}

TEST(IsisEventNexusLoaderTest, test_generate_spectrum_detector_mapping) {
  std::vector<int32_t> spec{
      1, 1, 1, 4, 4, 5, 5, 5, 6, 6, 6,
  };

  std::vector<int32_t> udet{
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  };

  EXPECT_EQ(spec.size(), udet.size());

  SpectrumToDetectorMapping mapping;
  generate_spectrum_detector_mapping(mapping, spec, udet);

  EXPECT_EQ(4, mapping.size());

  {
    std::vector<detid_t> expected{1, 2, 3};
    EXPECT_EQ(expected, mapping[1]);
  }

  {
    std::vector<detid_t> expected{4, 5};
    EXPECT_EQ(expected, mapping[4]);
  }

  {
    std::vector<detid_t> expected{6, 7, 8};
    EXPECT_EQ(expected, mapping[5]);
  }

  {
    std::vector<detid_t> expected{9, 10, 11};
    EXPECT_EQ(expected, mapping[6]);
  }
}

TEST(IsisEventNexusLoaderTest,
     test_generate_spectrum_detector_mapping_non_equal_dimensions) {
  std::vector<int32_t> spec{
      1, 1, 1, 4, 4, 5, 5, 5, 6, 6, 6,
  };

  std::vector<int32_t> udet{
      1, 2, 3, 4, 5, 6, 7, 8, 9,
  };

  SpectrumToDetectorMapping mapping;
  generate_spectrum_detector_mapping(mapping, spec, udet);

  EXPECT_EQ(0, mapping.size());
}
