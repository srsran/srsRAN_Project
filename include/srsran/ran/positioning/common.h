/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

namespace srsran {

enum class report_characteristics_t { on_demand, periodic };

enum class meas_periodicity_t : uint32_t {
  ms120   = 120,
  ms240   = 240,
  ms480   = 480,
  ms640   = 640,
  ms1024  = 1024,
  ms2048  = 2048,
  ms5120  = 5120,
  ms10240 = 10240,
  min1    = 1,
  min6    = 6,
  min12   = 12,
  min30   = 30,
  min60   = 60,
  ms20480 = 20480,
  ms40960 = 40960,
};

struct lcs_to_gcs_translation_t {
  uint16_t alpha;
  uint16_t beta;
  uint16_t gamma;
};

enum class latitude_sign_t { north, south };

enum class direction_of_altitude_t { height, depth };

/// The configured estimated geographical position of the antenna of the cell/TP.
struct ng_ran_access_point_position_t {
  latitude_sign_t         latitude_sign;
  uint32_t                latitude;
  int32_t                 longitude;
  direction_of_altitude_t direction_of_altitude;
  uint16_t                altitude;
  uint8_t                 uncertainty_semi_major;
  uint8_t                 uncertainty_semi_minor;
  uint8_t                 orientation_of_major_axis;
  uint8_t                 uncertainty_altitude;
  uint8_t                 confidence;
};

} // namespace srsran
