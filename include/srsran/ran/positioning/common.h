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

#include "srsran/ran/csi_rs/csi_rs_id.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/scs_specific_carrier.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/ran/ssb/ssb_properties.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <cstdint>
#include <optional>
#include <variant>
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

// Time in seconds relative to 00:00:00 on 1 January 1900 (calculated as continuous time without leap seconds and
// traceable to a common time reference) where binary encoding of the integer part is in the first 32 bits and binary
// encoding of the fraction part in the last 32 bits. The fraction part is expressed with a granularity of 1 /2**32
// second
using relative_time_1900_t = uint64_t;

struct tf_config_t {
  uint32_t                            ssb_freq;
  subcarrier_spacing                  ssb_subcarrier_spacing;
  int8_t                              ssb_tx_pwr;
  ssb_periodicity                     ssb_period;
  uint8_t                             ssb_half_frame_offset;
  uint8_t                             ssb_sfn_offset;
  std::optional<ssb_burst_position_t> ssb_burst_position;
  std::optional<relative_time_1900_t> sfn_initialization_time;
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

  bool operator==(const ssb_t& rhs) const { return pci_nr == rhs.pci_nr && ssb_idx == rhs.ssb_idx; }
  bool operator!=(const ssb_t& rhs) const { return !(rhs == *this); }
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

struct dl_prs_t {
  uint16_t               prs_id;
  uint8_t                dl_prs_res_set_id;
  std::optional<uint8_t> dl_prs_res_id;

  bool operator==(const dl_prs_t& rhs) const
  {
    return prs_id == rhs.prs_id && dl_prs_res_set_id == rhs.dl_prs_res_set_id && dl_prs_res_id == rhs.dl_prs_res_id;
  }
  bool operator!=(const dl_prs_t& rhs) const { return !(rhs == *this); }
};

struct spatial_relation_info_t {
  using reference_signal =
      std::variant<nzp_csi_rs_res_id_t, ssb_t, srs_config::srs_res_id, srs_config::srs_pos_res_id, dl_prs_t>;

  std::vector<reference_signal> reference_signals;

  bool operator==(const spatial_relation_info_t& rhs) const { return reference_signals == rhs.reference_signals; }
  bool operator!=(const spatial_relation_info_t& rhs) const { return !(rhs == *this); }
};

struct active_ul_bwp_t {
  uint16_t            location_and_bw;
  subcarrier_spacing  scs;
  cyclic_prefix       cp;
  uint16_t            tx_direct_current_location;
  std::optional<bool> shift7dot5k_hz;
  srs_config          srs_cfg;

  bool operator==(const active_ul_bwp_t& rhs) const
  {
    return location_and_bw == rhs.location_and_bw && scs == rhs.scs && cp == rhs.cp &&
           tx_direct_current_location == rhs.tx_direct_current_location && shift7dot5k_hz == rhs.shift7dot5k_hz &&
           srs_cfg == rhs.srs_cfg;
  }
  bool operator!=(const active_ul_bwp_t& rhs) const { return !(rhs == *this); }
};

struct srs_carrier_list_item_t {
  uint32_t                          point_a;
  std::vector<scs_specific_carrier> ul_ch_bw_per_scs_list;
  active_ul_bwp_t                   active_ul_bwp;
  std::optional<pci_t>              pci_nr;

  bool operator==(const srs_carrier_list_item_t& rhs) const
  {
    return point_a == rhs.point_a && ul_ch_bw_per_scs_list == rhs.ul_ch_bw_per_scs_list &&
           active_ul_bwp == rhs.active_ul_bwp && pci_nr == rhs.pci_nr;
  }
  bool operator!=(const srs_carrier_list_item_t& rhs) const { return !(rhs == *this); }
};

struct srs_configuration_t {
  std::vector<srs_carrier_list_item_t> srs_carrier_list;

  bool operator==(const srs_configuration_t& rhs) const { return srs_carrier_list == rhs.srs_carrier_list; }
  bool operator!=(const srs_configuration_t& rhs) const { return !(rhs == *this); }
};

struct trp_tx_teg_info_t {
  uint8_t trp_tx_teg_id;
  uint8_t trp_tx_timing_error_margin;
};

} // namespace srsran
