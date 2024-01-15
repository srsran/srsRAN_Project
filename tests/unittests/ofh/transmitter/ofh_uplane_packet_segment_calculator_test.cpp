/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "../../../../lib/ofh/transmitter/ofh_uplane_fragment_size_calculator.h"
#include "srsran/adt/bounded_bitset.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

/// Returns number of segments after segmenting the given number of PRBs into frames of given size.
static unsigned segment_prbs(const ru_compression_params& comp_params, unsigned nof_prbs, units::bytes frame_size)
{
  ofh_uplane_fragment_size_calculator calculator(0, nof_prbs, comp_params);

  bool     calculate_segments_finished = false;
  unsigned nof_segments                = 0;
  unsigned segment_start_prb           = 0;
  unsigned segment_nof_prbs            = 0;

  while (!calculate_segments_finished) {
    calculate_segments_finished =
        calculator.calculate_fragment_size(segment_start_prb, segment_nof_prbs, frame_size.value());
    ++nof_segments;
  }
  return nof_segments;
}

TEST(ofh_uplane_packet_fragment_calculator, prbs_fit_in_one_segment_passes)
{
  ru_compression_params comp_params;
  comp_params.type       = compression_type::none;
  comp_params.data_width = 16;
  unsigned start_prb     = 0;
  unsigned nof_prbs      = 10;

  ofh_uplane_fragment_size_calculator calculator(start_prb, nof_prbs, comp_params);
  unsigned                            nof_segments = 1;

  unsigned jumbo_frame_size  = 9600;
  unsigned segment_start_prb = 0;
  unsigned segment_nof_prbs  = 0;
  while (!calculator.calculate_fragment_size(segment_start_prb, segment_nof_prbs, jumbo_frame_size)) {
    ++nof_segments;
  }

  ASSERT_EQ(nof_segments, 1);
  ASSERT_EQ(nof_prbs, segment_nof_prbs);
  ASSERT_EQ(segment_start_prb, segment_start_prb);
}

TEST(ofh_uplane_packet_fragment_calculator, whole_prb_range_compressed_passes)
{
  ru_compression_params comp_params;
  comp_params.type       = compression_type::BFP;
  comp_params.data_width = 9;
  unsigned start_prb     = 0;
  unsigned nof_prbs      = 273;

  ofh_uplane_fragment_size_calculator calculator(start_prb, nof_prbs, comp_params);
  unsigned                            nof_segments = 1;

  unsigned jumbo_frame_size  = 9600;
  unsigned segment_start_prb = 0;
  unsigned segment_nof_prbs  = 0;
  while (!calculator.calculate_fragment_size(segment_start_prb, segment_nof_prbs, jumbo_frame_size)) {
    ++nof_segments;
  }

  ASSERT_EQ(nof_segments, 1);
  ASSERT_EQ(nof_prbs, segment_nof_prbs);
  ASSERT_EQ(segment_start_prb, segment_start_prb);
}

TEST(ofh_uplane_packet_fragment_calculator, whole_prb_range_uncompressed_generates_two_segments)
{
  ru_compression_params comp_params;
  comp_params.type       = compression_type::none;
  comp_params.data_width = 16;
  unsigned start_prb     = 0;
  unsigned nof_prbs      = 273;

  ofh_uplane_fragment_size_calculator calculator(start_prb, nof_prbs, comp_params);
  unsigned                            nof_segments = 0;

  unsigned jumbo_frame_size  = 9600;
  unsigned segment_start_prb = 0;
  unsigned segment_nof_prbs  = 0;
  bool     is_last_packet    = false;
  do {
    is_last_packet = calculator.calculate_fragment_size(segment_start_prb, segment_nof_prbs, jumbo_frame_size);
    ++nof_segments;

    ASSERT_EQ(segment_start_prb, (nof_segments == 1) ? 0 : 200);
    ASSERT_EQ(segment_nof_prbs, (nof_segments == 1) ? 200 : 73);
  } while (!is_last_packet);

  ASSERT_EQ(nof_segments, 2);
}

TEST(ofh_uplane_packet_fragment_calculator, smaller_frame_size_than_prb_returns_zero_prbs)
{
  ru_compression_params comp_params;
  comp_params.type       = compression_type::none;
  comp_params.data_width = 16;
  unsigned start_prb     = 33;
  unsigned nof_prbs      = 3;

  ofh_uplane_fragment_size_calculator calculator(start_prb, nof_prbs, comp_params);

  unsigned frame_size        = 24;
  unsigned segment_start_prb = 0;
  unsigned segment_nof_prbs  = 0;
  calculator.calculate_fragment_size(segment_start_prb, segment_nof_prbs, frame_size);

  ASSERT_EQ(segment_nof_prbs, 0);
  ASSERT_EQ(segment_start_prb, start_prb);
}

TEST(ofh_uplane_packet_fragment_calculator, segmented_packets_covers_whole_prb_range)
{
  ru_compression_params comp_params;
  comp_params.type              = compression_type::none;
  comp_params.data_width        = 16;
  unsigned            start_prb = 0;
  unsigned            nof_prbs  = 273;
  bounded_bitset<273> used_prbs(273);

  ofh_uplane_fragment_size_calculator calculator(start_prb, nof_prbs, comp_params);
  unsigned                            nof_segments                = 0;
  bool                                calculate_segments_finished = false;

  unsigned jumbo_frame_size  = 9600;
  unsigned segment_start_prb = 0;
  unsigned segment_nof_prbs  = 0;
  while (!calculate_segments_finished) {
    calculate_segments_finished =
        calculator.calculate_fragment_size(segment_start_prb, segment_nof_prbs, jumbo_frame_size);
    ++nof_segments;

    for (unsigned i = segment_start_prb, e = segment_start_prb + segment_nof_prbs; i != e; ++i) {
      used_prbs.set(i);
    }
  }

  ASSERT_EQ(nof_segments, 2);
  ASSERT_EQ(used_prbs.count(), nof_prbs);
}

TEST(ofh_uplane_packet_fragment_calculator, different_frame_size_segments)
{
  ru_compression_params comp_params;
  comp_params.type              = compression_type::none;
  comp_params.data_width        = 16;
  unsigned            start_prb = 10;
  unsigned            nof_prbs  = 127;
  bounded_bitset<273> used_prbs(273);

  ofh_uplane_fragment_size_calculator calculator(start_prb, nof_prbs, comp_params);
  unsigned                            nof_segments                = 0;
  bool                                calculate_segments_finished = false;

  std::vector<unsigned> frame_sizes       = {60, 108, 204, 396, 780, 1548, 3084};
  unsigned              segment_start_prb = 0;
  unsigned              segment_nof_prbs  = 0;
  while (!calculate_segments_finished) {
    calculate_segments_finished =
        calculator.calculate_fragment_size(segment_start_prb, segment_nof_prbs, frame_sizes[nof_segments]);

    for (unsigned i = segment_start_prb, e = segment_start_prb + segment_nof_prbs; i != e; ++i) {
      used_prbs.set(i);
    }

    ASSERT_EQ(1U << nof_segments, segment_nof_prbs);

    ++nof_segments;
  }

  ASSERT_EQ(nof_segments, 7);
  for (unsigned i = start_prb, e = start_prb + nof_prbs; i != e; ++i) {
    ASSERT_TRUE(used_prbs.test(i));
  }
}

TEST(ofh_uplane_packet_fragment_calculator, correct_number_of_segments_calculated)
{
  ru_compression_params comp_params;
  comp_params.type       = compression_type::none;
  comp_params.data_width = 16;
  unsigned     nof_prbs  = 273;
  units::bytes headers_size{36};

  // Test jumbo frames, medium size frames and normal frames.
  units::bytes test_frame_sizes[] = {units::bytes{9000}, units::bytes{6600}, units::bytes{3200}, units::bytes{1500}};

  for (auto frame_size : test_frame_sizes) {
    unsigned nof_segments_processed = segment_prbs(comp_params, nof_prbs, frame_size - headers_size);
    unsigned nof_segments_calculated =
        ofh_uplane_fragment_size_calculator::calculate_nof_segments(frame_size, nof_prbs, comp_params, headers_size);
    ASSERT_EQ(nof_segments_processed, nof_segments_calculated);
  }
}
