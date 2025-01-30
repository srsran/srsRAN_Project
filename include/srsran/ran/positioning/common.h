/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/pci.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <cstdint>
#include <optional>
#include <vector>

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

struct ssb_burst_position_t {
  enum class bitmap_type_t : uint8_t { /* uint8_t */ short_bitmap,
                                       /* uint8_t */ medium_bitmap,
                                       /* uint64_t */ long_bitmap };

  bitmap_type_t type;
  uint64_t      bitmap;
};

struct tf_config_t {
  uint32_t                            ssb_freq;
  subcarrier_spacing                  ssb_subcarrier_spacing;
  int8_t                              ssb_tx_pwr;
  uint8_t                             ssb_periodicity;
  uint8_t                             ssb_half_frame_offset;
  uint8_t                             ssb_sfn_offset;
  std::optional<ssb_burst_position_t> ssb_burst_position;
  std::optional<uint64_t>             sfn_initialization_time;
};

struct ssb_info_item_t {
  tf_config_t ssb_cfg;
  pci_t       pci_nr;
};

struct ssb_info_t {
  std::vector<ssb_info_item_t> list_of_ssb_info;
};

struct ssb_t {
  pci_t                  pci_nr;
  std::optional<uint8_t> ssb_idx;
};

struct time_stamp_slot_idx_t {
  enum class idx_type { scs15, scs30, scs60, scs120 };

  idx_type type;
  uint8_t  value;
};

struct time_stamp_t {
  uint16_t                sys_frame_num;
  time_stamp_slot_idx_t   slot_idx;
  std::optional<uint64_t> meas_time;
};

} // namespace srsran
