/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
