/*
 * Space filling curve prototype for MD event data structure
 * Copyright (C) 2018 European Spallation Source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "MantidEventNexusLoader.h"

TEST(MantidEventNexusLoaderTest, EventsWithNoWeights) {
  MantidEventNexusLoader loader("test_data/mantid_event_no_weight.nxs");

  /* Events should note have weights */
  EXPECT_FALSE(loader.eventsHaveWeight());

  /* Should have 15 events */
  EXPECT_EQ(15, loader.totalEventCount());

  /* Load events */
  std::vector<TofEvent> events;
  loader.loadAllEvents(events);
  EXPECT_EQ(15, events.size());

  std::vector<size_t> expectedIds{0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3};

  std::vector<float> expectedTofs{0.01, 0.011, 0.012, 0.013, 0.014,
                                  0.02, 0.021, 0.022, 0.025, 0.029,
                                  0.03, 0.031, 0.034, 0.04,  0.05};

  /* Verify loaded event data */
  for (size_t i = 0; i < expectedTofs.size(); i++) {
    EXPECT_EQ(expectedIds[i], events[i].id);
    EXPECT_EQ(expectedTofs[i], events[i].tof);
    EXPECT_EQ(1.0f, events[i].weight);
  }
}

TEST(MantidEventNexusLoaderTest, EventsWithWeights) {
  MantidEventNexusLoader loader("test_data/mantid_event.nxs");

  /* Events should have weights */
  EXPECT_TRUE(loader.eventsHaveWeight());

  /* Should have 15 events */
  EXPECT_EQ(15, loader.totalEventCount());

  /* Load events */
  std::vector<TofEvent> events;
  loader.loadAllEvents(events);
  EXPECT_EQ(15, events.size());

  std::vector<size_t> expectedIds{0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3};

  std::vector<float> expectedTofs{0.01, 0.011, 0.012, 0.013, 0.014,
                                  0.02, 0.021, 0.022, 0.025, 0.029,
                                  0.03, 0.031, 0.034, 0.04,  0.05};

  std::vector<float> expectedWeights{0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55,
                                     0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9};

  /* Verify loaded event data */
  for (size_t i = 0; i < expectedTofs.size(); i++) {
    EXPECT_EQ(expectedIds[i], events[i].id);
    EXPECT_EQ(expectedTofs[i], events[i].tof);
    EXPECT_EQ(expectedWeights[i], events[i].weight);
  }
}

TEST(MantidEventNexusLoaderTest, SpectrumDetectorMapping) {
  MantidEventNexusLoader loader("test_data/mantid_event.nxs");

  /* Load spectrum to detector mapping */
  SpectrumToDetectorMapping specDetMap;
  loader.loadSpectrumDetectorMapping(specDetMap);

  /* Should have 4 spectra */
  EXPECT_EQ(4, specDetMap.size());

  /* Verify mapping */
  {
    const std::vector<detid_t> expected{101, 102, 103};
    EXPECT_EQ(expected, specDetMap[0]);
  }
  {
    const std::vector<detid_t> expected{104, 105, 106};
    EXPECT_EQ(expected, specDetMap[1]);
  }
  {
    const std::vector<detid_t> expected{107, 108, 109};
    EXPECT_EQ(expected, specDetMap[2]);
  }
  {
    const std::vector<detid_t> expected{110, 111, 112};
    EXPECT_EQ(expected, specDetMap[3]);
  }
}
