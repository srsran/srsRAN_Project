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
  int  ta_common_drift;
  unsigned ta_common_drift_variant;
};

struct epoch_time_t {
  unsigned sfn;
  unsigned subframe_number;
};

struct ntn_config {
  // sib 19 values
  std::string reference_location = "0";
  unsigned    distance_threshold = 0;
  // ntn-config values
  epoch_time_t        epoch_time            = {0, 0};
  unsigned            cell_specific_koffset = 0;
  unsigned            k_mac                 = 0;
  position_velocity_t ephemeris_info        = {1, 2, 3, 1, 1, 1};
  ta_common_t         ta_info               = {0, 0, 0};
};
} // namespace srsran
