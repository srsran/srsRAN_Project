/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cu_cp_unit_logger_config.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/five_qi.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/nr_band.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/s_nssai.h"
#include <vector>

namespace srsran {

/// Report configuration, for now only supporting the A3 event.
struct cu_cp_report_unit_config {
  unsigned           report_cfg_id;
  std::string        report_type;
  optional<unsigned> report_interval_ms;
  std::string        a3_report_type;
  /// [-30..30] Note the actual value is field value * 0.5 dB. E.g. putting a value of -6 here results in -3dB offset.
  optional<int>      a3_offset_db;
  optional<unsigned> a3_hysteresis_db;
  optional<unsigned> a3_time_to_trigger_ms;
};

struct cu_cp_neighbor_cell_unit_config_item {
  /// Cell id.
  uint64_t nr_cell_id;
  /// Report config ids.
  std::vector<uint64_t> report_cfg_ids;
};

/// Each item describes the relationship between one cell to all other cells.
struct cu_cp_cell_unit_config_item {
  /// Cell id.
  uint64_t           nr_cell_id;
  optional<unsigned> periodic_report_cfg_id;

  // These parameters must only be set for external cells
  /// gNodeB identifier bit length.
  optional<unsigned> gnb_id_bit_length;
  /// PCI.
  optional<pci_t> pci;
  /// NR band.
  optional<nr_band> band;
  /// SSB ARFCN.
  optional<unsigned> ssb_arfcn;
  /// SSB subcarrier spacing.
  optional<unsigned> ssb_scs;
  /// SSB period.
  optional<unsigned> ssb_period;
  /// SSB offset.
  optional<unsigned> ssb_offset;
  /// SSB duration.
  optional<unsigned> ssb_duration;
  /// Vector of cells that are a neighbor of this cell.
  std::vector<cu_cp_neighbor_cell_unit_config_item> ncells;
  // TODO: Add optional SSB parameters.
};

/// All mobility related configuration parameters.
struct mobility_unit_config {
  /// List of all cells known to the CU-CP.
  std::vector<cu_cp_cell_unit_config_item> cells;
  /// Report config.
  std::vector<cu_cp_report_unit_config> report_configs;
  /// Whether to start HO if neighbor cell measurements arrive.
  bool trigger_handover_from_measurements = false;
};

/// RRC specific configuration parameters.
struct rrc_cu_cp_unit_config {
  bool force_reestablishment_fallback = false;
  /// Timeout for RRC procedures (2 * default SRB maxRetxThreshold * t-PollRetransmit = 2 * 8 * 45ms = 720ms, see
  /// TS 38.331 Sec 9.2.1).
  unsigned rrc_procedure_timeout_ms = 720;
};

/// Security configuration parameters.
struct security_unit_config {
  std::string integrity_protection       = "not_needed";
  std::string confidentiality_protection = "required";
  std::string nea_preference_list        = "nea0,nea2,nea1,nea3";
  std::string nia_preference_list        = "nia2,nia1,nia3";
};

/// F1AP-CU configuration parameters.
struct f1ap_cu_unit_config {
  /// Timeout for the UE context setup procedure in milliseconds.
  unsigned ue_context_setup_timeout = 1000;
};

/// RLC UM TX configuration
struct rlc_tx_um_cu_cp_unit_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// RLC SDU queue size.
  uint32_t queue_size;
};

/// RLC UM RX configuration
struct rlc_rx_um_cu_cp_unit_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used by rx to detect PDU loss (ms).
  int32_t t_reassembly;
};

/// RLC UM configuration
struct rlc_um_cu_cp_unit_config {
  rlc_tx_um_cu_cp_unit_config tx;
  rlc_rx_um_cu_cp_unit_config rx;
};

/// RLC UM TX configuration
struct rlc_tx_am_cu_cp_unit_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Poll retx timeout (ms).
  int32_t t_poll_retx;
  /// Max retx threshold.
  uint32_t max_retx_thresh;
  /// Insert poll bit after this many PDUs.
  int32_t poll_pdu;
  /// Insert poll bit after this much data (bytes).
  int32_t poll_byte;
  /// Custom parameter to limit the maximum window size for memory reasons. 0 means no limit.
  uint32_t max_window = 0;
  /// RLC SDU queue size.
  uint32_t queue_size = 4096;
};

/// RLC UM RX configuration
struct rlc_rx_am_cu_cp_unit_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used by rx to detect PDU loss (ms).
  int32_t t_reassembly;
  /// Timer used by rx to prohibit tx of status PDU (ms).
  int32_t t_status_prohibit;

  /// Implementation-specific parameters that are not specified by 3GPP

  /// Maximum number of visited SNs in the RX window when building a status report. 0 means no limit.
  uint32_t max_sn_per_status = 0;
};

/// RLC AM configuration
struct rlc_am_cu_cp_unit_config {
  rlc_tx_am_cu_cp_unit_config tx;
  rlc_rx_am_cu_cp_unit_config rx;
};

/// RLC configuration
struct rlc_cu_cp_unit_config {
  std::string              mode = "am";
  rlc_um_cu_cp_unit_config um;
  rlc_am_cu_cp_unit_config am;
};

struct pdcp_rx_cu_cp_unit_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used to detect PDUs losses (ms).
  int32_t t_reordering;
  /// Whether out-of-order delivery to upper layers is enabled.
  bool out_of_order_delivery;
};

struct pdcp_tx_cu_cp_unit_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used to notify lower layers to discard PDUs (ms).
  int32_t discard_timer;
  /// Whether PDCP status report is required.
  bool status_report_required;
};

struct pdcp_cu_cp_unit_config {
  /// Whether DRB integrity is required.
  bool                      integrity_protection_required;
  pdcp_tx_cu_cp_unit_config tx;
  pdcp_rx_cu_cp_unit_config rx;
};

/// QoS configuration.
struct qos_cu_cp_unit_config {
  five_qi_t              five_qi = uint_to_five_qi(9);
  rlc_cu_cp_unit_config  rlc;
  pdcp_cu_cp_unit_config pdcp;
};

/// Metrics configuration.
struct metrics_cu_cp_unit_config {
  /// Statistics report period in seconds
  unsigned cu_cp_statistics_report_period = 1;
};

/// CU-CP application unit configuration.
struct cu_cp_unit_config {
  /// Loggers configuration.
  cu_cp_unit_logger_config loggers;
  /// Node name.
  std::string ran_node_name = "cu_cp_01";
  /// gNB identifier.
  gnb_id_t gnb_id = {411, 22};
  /// Maximum number of DUs.
  uint16_t max_nof_dus = 6;
  /// Maximum number of CU-UPs.
  uint16_t max_nof_cu_ups = 6;
  /// Inactivity timer in seconds.
  int inactivity_timer = 120;
  /// PDU session setup timeout in seconds (must be larger than T310).
  unsigned pdu_session_setup_timeout = 3;
  /// Metrics configuration.
  metrics_cu_cp_unit_config metrics;
  /// Mobility configuration.
  mobility_unit_config mobility_config;
  /// RRC configuration.
  rrc_cu_cp_unit_config rrc_config;
  /// Security configuration.
  security_unit_config security_config;
  /// F1-AP configuration.
  f1ap_cu_unit_config f1ap_config;
  /// QoS configuration.
  std::vector<qos_cu_cp_unit_config> qos_cfg;
  /// Network slice configuration.
  std::vector<s_nssai_t> slice_cfg = {s_nssai_t{1}};
};

} // namespace srsran
