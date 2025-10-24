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

#include "../common/interface_management.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/tac.h"
#include "srsran/support/async/async_task.h"
#include <optional>

namespace srsran {
namespace srs_du {

struct f1_reset_request {
  enum class cause_type { cell_removal, other };

  /// UEs to reset. If empty, all UEs will be reset.
  std::vector<du_ue_index_t> ues_reset;
  /// Cause for the F1 Reset.
  cause_type cause;
};

struct f1_reset_acknowledgement {
  bool success = true;
};

/// System Information Update from the gNB-DU.
struct gnb_du_sys_info {
  byte_buffer packed_mib;
  byte_buffer packed_sib1;
};

/// Information of served cell being added/modified.
struct du_served_cell_info {
  nr_cell_global_id_t                  nr_cgi;
  pci_t                                pci;
  tac_t                                tac;
  duplex_mode                          duplx_mode;
  subcarrier_spacing                   scs_common;
  carrier_configuration                dl_carrier;
  std::optional<carrier_configuration> ul_carrier;
  byte_buffer                          packed_meas_time_cfg;
  std::chrono::milliseconds            ntn_link_rtt = std::chrono::milliseconds(0);
};

/// \brief Served cell configuration that will be passed to CU-CP.
struct f1_cell_setup_params {
  // DU-internal identifier of the cell.
  du_cell_index_t        cell_index;
  du_served_cell_info    cell_info;
  gnb_du_sys_info        du_sys_info;
  std::vector<s_nssai_t> slices;
};

/// \brief Message that initiates a F1 Setup procedure.
struct f1_setup_request_message {
  gnb_du_id_t                       gnb_du_id;
  std::string                       gnb_du_name;
  uint8_t                           rrc_version;
  std::vector<f1_cell_setup_params> served_cells;
  unsigned                          max_setup_retries = 5;
};

/// Outcome of the F1 Setup procedure.
struct f1_setup_success {
  std::vector<f1ap_cell_to_activate> cells_to_activate;
};
struct f1_setup_failure {
  /// Possible result outcomes for F1 Setup failure.
  enum class result_code { timeout, proc_failure, invalid_response, f1_setup_failure };

  /// Result outcome for F1 Setup failure.
  result_code result;
  /// Cause provided by CU-CP in case of F1 Setup Failure.
  std::string f1_setup_failure_cause;
};
using f1_setup_result = expected<f1_setup_success, f1_setup_failure>;

/// Cell whose parameters need to be modified in the DU.
struct f1ap_cell_to_be_modified {
  /// New served Cell Information.
  du_served_cell_info cell_info;
  /// New System Information.
  std::optional<gnb_du_sys_info> du_sys_info;
};

/// gNB-DU initiated Config Update as per TS 38.473, Section 8.2.4.
struct gnbdu_config_update_request {
  std::vector<f1ap_cell_to_be_modified> cells_to_mod;
};

struct gnbdu_config_update_response {
  bool result;
};

/// gNB-CU initiated Config Update as per TS 38.473, Section 8.2.5.
/// Request sent by CU to DU via F1AP gNB-CU Configuration Update.
struct gnbcu_config_update_request {
  std::vector<f1ap_cell_to_activate> cells_to_activate;
  std::vector<nr_cell_global_id_t>   cells_to_deactivate;
};

/// Result of gNB-CU initiated Config Update as per TS 38.473, Section 8.2.5.
struct gnbcu_config_update_response {
  std::vector<nr_cell_global_id_t> cells_failed_to_activate;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_connection_manager
{
public:
  virtual ~f1ap_connection_manager() = default;

  /// \brief Connect the DU to CU-CP via F1-C interface.
  [[nodiscard]] virtual bool connect_to_cu_cp() = 0;

  /// \brief Initiates the F1 Setup procedure as per TS 38.473, Section 8.2.3.
  /// \param[in] request The F1SetupRequest message to transmit.
  /// \return Returns a f1_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise. \remark The DU transmits the F1SetupRequest as per TS 38.473 section 8.2.3
  /// and awaits the response. If a F1SetupFailure is received the F1AP will handle the failure.
  virtual async_task<f1_setup_result> handle_f1_setup_request(const f1_setup_request_message& request) = 0;

  /// \brief Launches the F1 Removal procedure as per TS 38.473, Section 8.2.8.
  virtual async_task<void> handle_f1_removal_request() = 0;

  /// \brief Initiates F1AP reset procedure as per TS 38.473, Section 8.2.1.2.2.
  virtual async_task<f1_reset_acknowledgement> handle_f1_reset_request(const f1_reset_request& req) = 0;

  /// \brief Initiates F1AP gNB-DU config update procedure as per TS 38.473, Section 8.2.4.
  virtual async_task<gnbdu_config_update_response>
  handle_du_config_update(const gnbdu_config_update_request& request) = 0;

  /// \brief Query whether the F1-C interface has been setup with the CU-CP.
  virtual bool is_f1_setup() const = 0;
};

/// Notifier used by F1AP to signal to the DU any CU-initiated requests related with the F1AP interface management.
class f1ap_interface_update_notifier
{
public:
  virtual ~f1ap_interface_update_notifier() = default;

  /// \brief Notify the DU of an update requested initiated by the CU via F1AP, as per TS 38.473, Section 8.2.5.
  virtual async_task<gnbcu_config_update_response>
  request_cu_context_update(const gnbcu_config_update_request& request) = 0;
};

} // namespace srs_du
} // namespace srsran
