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

#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/positioning/common.h"
#include "srsran/ran/positioning/measurement_information.h"
#include "srsran/ran/srs/srs_configuration.h"
#include <cstdint>
#include <variant>

namespace srsran {

struct res_type_periodic_t {
  uint16_t periodicity;
  uint16_t offset;
};

struct res_type_semi_persistent_t {
  uint16_t periodicity;
  uint16_t offset;
};

struct res_type_aperiodic_t {
  bool aperiodic_res_type;
};

using res_type_t = std::variant<res_type_periodic_t, res_type_semi_persistent_t, res_type_aperiodic_t>;

using bw_srs_t = std::variant<uint8_t, uint16_t>;

using pathloss_ref_sig_t = std::variant<ssb_t, dl_prs_t>;

struct pathloss_ref_info_t {
  pathloss_ref_sig_t pathloss_ref_sig;
};

struct periodicity_item_t {
  enum class value_t {
    ms0dot125 = 0,
    ms0dot25,
    ms0dot5,
    ms0dot625,
    ms1,
    ms1dot25,
    ms2,
    ms2dot5,
    ms4dot,
    ms5,
    ms8,
    ms10,
    ms16,
    ms20,
    ms32,
    ms40,
    ms64,
    ms80m,
    ms160,
    ms320,
    ms640m,
    ms1280,
    ms2560,
    ms5120,
    ms10240,
  };

  static float to_float(const periodicity_item_t::value_t& enum_val)
  {
    static const float numbers[] = {0.125, 0.25,  0.5,   0.625,  1.0,    1.25,   2.0,    2.5,  4.0,
                                    5.0,   8.0,   10.0,  16.0,   20.0,   32.0,   40.0,   64.0, 80.0,
                                    160.0, 320.0, 640.0, 1280.0, 2560.0, 5120.0, 10240.0};

    if ((unsigned)enum_val >= sizeof(numbers)) {
      return (float)0;
    }
    return numbers[(uint8_t)enum_val];
  }

  value_t value;
};

struct srs_res_set_item_t {
  std::optional<uint8_t>                 nof_srs_res_per_set;
  std::vector<periodicity_item_t>        periodicity_list;
  std::optional<spatial_relation_info_t> spatial_relation_info;
  std::optional<pathloss_ref_info_t>     pathloss_ref_info;
  std::vector<spatial_relation_info_t>   spatial_relation_per_srs_res_list;
};

struct requested_srs_tx_characteristics_t {
  enum class res_type_t { periodic, semi_persistent, aperiodic };

  std::optional<uint16_t>         nof_txs;
  res_type_t                      res_type;
  bw_srs_t                        bw;
  std::vector<srs_res_set_item_t> list_of_srs_res_set;
  std::optional<ssb_info_t>       ssb_info;
  std::vector<uint32_t>           srs_freqs;
};

struct ue_report_info_t {
  uint8_t report_amount;
  uint8_t report_interv;
};

enum class ue_teg_info_request_t { on_demand, periodic, stop };

struct scs_specific_carrier_t {
  uint16_t           offset_to_carrier;
  subcarrier_spacing scs;
  uint16_t           carrier_bw;
};

struct active_ul_bwp_t {
  uint16_t            location_and_bw;
  subcarrier_spacing  scs;
  cyclic_prefix       cp;
  uint16_t            tx_direct_current_location;
  std::optional<bool> shift7dot5k_hz;
  srs_config          srs_cfg;
};

struct srs_carrier_list_item_t {
  uint32_t                            point_a;
  std::vector<scs_specific_carrier_t> ul_ch_bw_per_scs_list;
  active_ul_bwp_t                     active_ul_bwp;
  std::optional<pci_t>                pci_nr;
};

struct srs_configuration_t {
  std::vector<srs_carrier_list_item_t> srs_carrier_list;
};

struct carrier_freq_t {
  uint32_t point_a;
  uint16_t offset_to_carrier;
};

struct ue_tx_teg_assoc_item_t {
  uint8_t                       ue_tx_teg_id;
  std::vector<uint8_t>          pos_srs_res_id_list;
  time_stamp_t                  time_stamp;
  std::optional<carrier_freq_t> carrier_freq;
  std::vector<uint8_t>          timing_error_margins;
};

} // namespace srsran
