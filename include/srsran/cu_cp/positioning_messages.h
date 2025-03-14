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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/cause/nrppa_cause.h"
#include "srsran/ran/positioning/measurement_information.h"
#include "srsran/ran/positioning/positioning_activation.h"
#include "srsran/ran/positioning/positioning_information_exchange.h"
#include "srsran/ran/positioning/trp_information_exchange.h"
#include <optional>

namespace srsran::srs_cu_cp {

struct trp_information_request_t {
  // TRP list is optional.
  std::vector<trp_id_t>                    trp_list;
  std::vector<trp_information_type_item_t> trp_info_type_list_trp_req;
};

struct trp_information_response_t {
  std::vector<trp_information_list_trp_response_item_t> trp_info_list_trp_resp;
};

struct trp_information_failure_t {
  nrppa_cause_t cause;
};

struct measurement_request_t {
  lmf_meas_id_t                                lmf_meas_id;
  std::vector<trp_meas_request_item_t>         trp_meas_request_list;
  report_characteristics_t                     report_characteristics;
  std::optional<meas_periodicity_t>            meas_periodicity;
  std::vector<trp_meas_quantities_list_item_t> trp_meas_quantities;

  // TODO: Add missing optional values.
};

struct measurement_response_t {
  lmf_meas_id_t                                lmf_meas_id;
  ran_meas_id_t                                ran_meas_id;
  std::vector<trp_measurement_response_item_t> trp_meas_resp_list;
};

struct measurement_failure_t {
  lmf_meas_id_t lmf_meas_id;
  nrppa_cause_t cause;
};

struct measurement_report_t {
  lmf_meas_id_t                                lmf_meas_id;
  ran_meas_id_t                                ran_meas_id;
  std::vector<trp_measurement_response_item_t> trp_meas_report_list;
};

struct measurement_update_t {
  lmf_meas_id_t lmf_meas_id;
  ran_meas_id_t ran_meas_id;
  // Optional values:
  std::vector<trp_measurement_update_item_t> trp_meas_upd_list;
  // TODO: Add missing optional values.
};

struct measurement_abort_t {
  lmf_meas_id_t lmf_meas_id;
  ran_meas_id_t ran_meas_id;
};

struct positioning_information_request_t {
  ue_index_t ue_index;

  std::optional<requested_srs_tx_characteristics_t> requested_srs_tx_characteristics;
  std::optional<ue_report_info_t>                   ue_report_info;
  std::optional<ue_teg_info_request_t>              ue_teg_info_request;
  std::optional<uint32_t>                           ue_teg_report_periodicity;
};

struct positioning_information_response_t {
  std::optional<srs_configuration_t>  srs_cfg;
  std::optional<relative_time_1900_t> sfn_initialization_time;
  // Optional list.
  std::vector<ue_tx_teg_assoc_item_t> ue_tx_teg_assoc_list;
};

struct positioning_information_failure_t {
  nrppa_cause_t cause;
};

struct positioning_activation_request_t {
  ue_index_t ue_index;

  srs_type_t                          srs_type;
  std::optional<relative_time_1900_t> activation_time;
};

struct positioning_activation_response_t {
  std::optional<uint16_t> sys_frame_num;
  std::optional<uint8_t>  slot_num;
};

struct positioning_activation_failure_t {
  nrppa_cause_t cause;
};

} // namespace srsran::srs_cu_cp
