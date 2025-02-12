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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/cause/nrppa_cause.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/ran/positioning/measurement_information.h"
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
  std::optional<crit_diagnostics_t>                     crit_diagnostics;
};

struct trp_information_failure_t {
  nrppa_cause_t                     cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
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
  std::optional<crit_diagnostics_t>            crit_diagnostics;
};

struct measurement_failure_t {
  lmf_meas_id_t                     lmf_meas_id;
  nrppa_cause_t                     cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
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
  std::optional<srs_configuration_t>       srs_cfg;
  std::optional<sfn_initialization_time_t> sfn_initialization_time;
  std::optional<crit_diagnostics_t>        crit_diagnostics;
  // Optional list.
  std::vector<ue_tx_teg_assoc_item_t> ue_tx_teg_assoc_list;
};

struct positioning_information_failure_t {
  nrppa_cause_t                     cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

} // namespace srsran::srs_cu_cp
