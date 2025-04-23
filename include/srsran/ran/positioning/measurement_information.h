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

#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/positioning/common.h"
#include "srsran/ran/positioning/positioning_ids.h"
#include <optional>
#include <variant>
#include <vector>

namespace srsran {

struct search_win_info_t {
  int16_t expected_propagation_delay;
  uint8_t delay_uncertainty;
};

struct expected_azimuth_aoa_t {
  uint16_t expected_azimuth_aoa_value;
  uint16_t expected_azimuth_aoa_uncertainty;
};

struct expected_zenith_aoa_t {
  uint16_t expected_zenith_aoa_value;
  uint16_t expected_zenith_aoa_uncertainty;
};

struct expected_ul_aoa_t {
  expected_azimuth_aoa_t               expected_azimuth_aoa;
  std::optional<expected_zenith_aoa_t> expected_zenith_aoa;
};

struct expected_zoa_only_t {
  expected_zenith_aoa_t expected_zoa_only;
};

using angle_meas_type_t = std::variant<expected_ul_aoa_t, expected_zoa_only_t>;

struct aoa_assist_info_t {
  angle_meas_type_t                       angle_meas;
  std::optional<lcs_to_gcs_translation_t> lcs_to_gcs_translation;
};

struct trp_meas_request_item_t {
  trp_id_t                         trp_id;
  std::optional<search_win_info_t> search_win_info;
  // IE exts.
  std::optional<nr_cell_global_id_t> cgi_nr;
  std::optional<aoa_assist_info_t>   aoa_search_win;
  std::optional<uint8_t>             nof_trp_rx_teg;
  std::optional<uint8_t>             nof_trp_rx_tx_teg;
};

enum class trp_meas_quantities_item_t {
  gnb_rx_tx_time_diff,
  ul_srs_rsrp,
  ul_aoa,
  ul_rtoa,
  multiple_ul_aoa,
  ul_srs_rsrp_p,
};

struct trp_meas_quantities_list_item_t {
  trp_meas_quantities_item_t trp_meas_quantities_item;
  std::optional<uint8_t>     timing_report_granularity_factor;
};

struct response_time_t {
  enum class time_unit_t { second, ten_seconds, ten_milliseconds };

  uint8_t     time;
  time_unit_t time_unit;
};

struct ul_angle_of_arrival_t {
  uint16_t                                azimuth_aoa;
  std::optional<uint16_t>                 zenith_aoa;
  std::optional<lcs_to_gcs_translation_t> lcs_to_gcs_translation;
};

struct ul_rtoa_meas_t {
  enum class type_t { k0, k1, k2, k3, k4, k5 };

  type_t   type;
  uint32_t value;
};

struct relative_path_delay_t {
  enum class type_t { k0, k1, k2, k3, k4, k5 };

  type_t   type;
  uint16_t value;
};

struct trp_meas_timing_quality_t {
  struct resolution_t {
    enum value_t { m0dot1, m1, m10, m30 };

    static float to_float(const resolution_t::value_t& enum_val)
    {
      static const float numbers[] = {0.1, 1.0, 10.0, 30.0};

      if ((unsigned)enum_val >= sizeof(numbers)) {
        return (float)0;
      }
      return numbers[(uint8_t)enum_val];
    }

    value_t value;
  };

  uint8_t      meas_quality;
  resolution_t resolution;
};

struct trp_meas_angle_quality_t {
  struct resolution_t {
    enum class value_t { deg0dot1 };

    static float to_float() { return 0.1; }

    value_t value;
  };

  uint16_t                azimuth_quality;
  std::optional<uint16_t> zenith_quality;
  resolution_t            resolution;
};

using trp_meas_quality_t = std::variant<trp_meas_timing_quality_t, trp_meas_angle_quality_t>;

struct zoa_t {
  uint16_t                                zenith_aoa;
  std::optional<lcs_to_gcs_translation_t> lcs_to_gcs_translation;
};

using multiple_ul_aoa_item_t = std::variant<ul_angle_of_arrival_t, zoa_t>;

struct ul_srs_rsrp_pwr_t {
  uint8_t first_path_rsrp_pwr;
};

struct add_path_list_item_t {
  relative_path_delay_t             relative_time_of_path;
  std::optional<trp_meas_quality_t> path_quality;
  // Multiple AoA is optional.
  std::vector<multiple_ul_aoa_item_t> multiple_ul_aoa;
  std::optional<ul_srs_rsrp_pwr_t>    path_pwr;
};

struct trp_rx_teg_info_t {
  uint8_t trp_rx_teg_id;
  uint8_t trp_rx_timing_error_margin;
};

struct ul_rtoa_t {
  ul_rtoa_meas_t                    ul_rtoa_meas;
  std::vector<add_path_list_item_t> add_path_list;
  // Extended add path list is optional.
  std::vector<add_path_list_item_t> extended_add_path_list;
  std::optional<trp_rx_teg_info_t>  trp_rx_teg_info;
};

struct gnb_rx_tx_time_diff_meas_t {
  enum class type_t { k0, k1, k2, k3, k4, k5 };

  type_t   type;
  uint32_t value;
};

struct rx_tx_timing_error_margin_t {
  enum class value_t { tc0dot5, tc1, tc2, tc4, tc8, tc12, tc16, tc20, tc24, tc32, tc40, tc48, tc64, tc80, tc96, tc128 };

  static float to_float(const rx_tx_timing_error_margin_t::value_t& enum_val)
  {
    static const float numbers[] = {
        0.5, 1.0, 2.0, 4.0, 8.0, 12.0, 16.0, 20.0, 24.0, 32.0, 40.0, 48.0, 64.0, 80.0, 96.0, 128.0};

    if ((unsigned)enum_val >= sizeof(numbers)) {
      return (float)0;
    }
    return numbers[(uint8_t)enum_val];
  }

  value_t value;
};

struct trp_rx_tx_teg_info_t {
  uint16_t                    trp_rx_tx_teg_id;
  rx_tx_timing_error_margin_t trp_rx_tx_timing_error_margin;
};

struct rx_tx_teg_t {
  trp_rx_tx_teg_info_t             trp_rx_tx_teg_info;
  std::optional<trp_tx_teg_info_t> trp_tx_teg_info;
};

struct rx_teg_t {
  trp_rx_teg_info_t trp_rx_teg_info;
  trp_tx_teg_info_t trp_tx_teg_info;
};

using trp_teg_info_t = std::variant<rx_tx_teg_t, rx_teg_t>;

struct gnb_rx_tx_time_diff_t {
  gnb_rx_tx_time_diff_meas_t        rx_tx_time_diff;
  std::vector<add_path_list_item_t> add_path_list;
  // Extended add path list is optional.
  std::vector<add_path_list_item_t> extended_add_path_list;
  std::optional<trp_teg_info_t>     trp_teg_info;
};

using trp_measured_results_value_t = std::variant<ul_angle_of_arrival_t, uint8_t, ul_rtoa_t, gnb_rx_tx_time_diff_t>;

struct meas_beam_info_t {
  std::optional<uint8_t> prs_res_id;
  std::optional<uint8_t> prs_res_set_id;
  std::optional<uint8_t> ssb_idx;
};

struct trp_meas_result_item_t {
  trp_measured_results_value_t      measured_results_value;
  time_stamp_t                      time_stamp;
  std::optional<trp_meas_quality_t> meas_quality;
  std::optional<meas_beam_info_t>   meas_beam_info;
  // TODO: Add optional ext ies.
};

struct trp_measurement_response_item_t {
  trp_id_t                            trp_id;
  std::vector<trp_meas_result_item_t> meas_result;
  std::optional<nr_cell_global_id_t>  cgi_nr;
};

} // namespace srsran
