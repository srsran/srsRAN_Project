/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include <cstdint>
#include <string>

namespace srsran {

struct position_velocity_t {
  int position_x;
  int position_y;
  int position_z;
  int velocity_vx;
  int velocity_vy;
  int velocity_vz;
};

struct ta_common_t {
  unsigned ta_common;
  int      ta_common_drift;
  unsigned ta_common_drift_variant;
};

struct epoch_time_t {
  unsigned sfn;
  unsigned subframe_number;
};

struct ntn_config {
  // sib 19 values
  optional<std::string> reference_location;
  optional<unsigned>    distance_threshold;
  // ntn-config values
  optional<epoch_time_t> epoch_time;
  unsigned               cell_specific_koffset;
  optional<unsigned>     k_mac;
  position_velocity_t    ephemeris_info;
  optional<ta_common_t>  ta_info;
};
} // namespace srsran
