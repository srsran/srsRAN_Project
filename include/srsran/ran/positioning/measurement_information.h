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
  ul_ao_a,
  ul_rtoa,
  multiple_ul_ao_a,
  ul_srs_rsrp_p,
};

struct trp_meas_quantities_list_item_t {
  trp_meas_quantities_item_t trp_meas_quantities_item;
  std::optional<uint8_t>     timing_report_granularity_factor;
};

enum class trp_measured_results_value_t { ul_angle_of_arrival, ul_srs_rsrp, ul_rtoa, gnb_rx_tx_time_diff };

struct timing_meas_quality_t {
  enum class resolution_t { m0dot1, m1, m10, m30 };

  uint8_t      meas_quality;
  resolution_t resolution;
};

struct angle_meas_quality_t {
  enum class resolution_t { deg0dot1 };

  uint16_t                azimuth_quality;
  std::optional<uint16_t> zenith_quality;
  resolution_t            resolution;
};

using trp_meas_quality_t = std::variant<timing_meas_quality_t, angle_meas_quality_t>;

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

struct trp_measurement_update_item_t {
  trp_id_t                         trp_id;
  std::optional<aoa_assist_info_t> ao_a_win_info;
  std::optional<uint8_t>           nof_trp_rx_teg;
  std::optional<uint8_t>           nof_trp_rx_tx_teg;
};

} // namespace srsran
