/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_log_unit_config.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/nr_band.h"
#include "srsran/ran/pci.h"
#include <vector>

namespace srsran {

/// Report configuration, for now only supporting the A3 event.
struct cu_cp_report_unit_config {
  unsigned           report_cfg_id;
  std::string        report_type;
  optional<unsigned> report_interval_ms;
  std::string        a3_report_type;
  optional<int> a3_offset_db; ///< [-30..30] Note the actual value is field value * 0.5 dB. E.g. putting a value of -6
                              ///< here results in -3dB offset.
  optional<unsigned> a3_hysteresis_db;
  optional<unsigned> a3_time_to_trigger_ms;
};

struct cu_cp_neighbor_cell_unit_config_item {
  uint64_t              nr_cell_id;     ///< Cell id.
  std::vector<uint64_t> report_cfg_ids; ///< Report config ids
};

/// Each item describes the relationship between one cell to all other cells.
struct cu_cp_cell_unit_config_item {
  uint64_t           nr_cell_id; ///< Cell id.
  optional<unsigned> periodic_report_cfg_id;

  // These parameters must only be set for external cells
  // TODO: Add optional SSB parameters.
  optional<unsigned> gnb_id_bit_length; ///< gNodeB identifier bit length.
  optional<pci_t>    pci;               ///< PCI.
  optional<nr_band>  band;              ///< NR band.
  optional<unsigned> ssb_arfcn;         ///< SSB ARFCN.
  optional<unsigned> ssb_scs;           ///< SSB subcarrier spacing.
  optional<unsigned> ssb_period;        ///< SSB period.
  optional<unsigned> ssb_offset;        ///< SSB offset.
  optional<unsigned> ssb_duration;      ///< SSB duration.

  std::vector<cu_cp_neighbor_cell_unit_config_item> ncells; ///< Vector of cells that are a neighbor of this cell.
};

/// All mobility related configuration parameters.
struct mobility_unit_config {
  std::vector<cu_cp_cell_unit_config_item> cells;          ///< List of all cells known to the CU-CP.
  std::vector<cu_cp_report_unit_config>    report_configs; ///< Report config.
  bool trigger_handover_from_measurements = false; ///< Whether to start HO if neighbor cell measurements arrive.
};

/// RRC specific configuration parameters.
struct rrc_appconfig {
  bool     force_reestablishment_fallback = false;
  unsigned rrc_procedure_timeout_ms       = 720; ///< Timeout for RRC procedures (2 * default SRB maxRetxThreshold *
                                                 ///< t-PollRetransmit = 2 * 8 * 45ms = 720ms, see TS 38.331 Sec 9.2.1).
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

/// CU-CP application configuration.
struct cu_cp_unit_config {
  // :TODO: not sure if gNB id should be here.
  gnb_id_t gnb_id = {411, 22};
  // :TODO: AMF should be here too?
  uint16_t              max_nof_dus               = 6;
  uint16_t              max_nof_cu_ups            = 6;
  int                   inactivity_timer          = 5; // in seconds
  unsigned              pdu_session_setup_timeout = 3; // in seconds (must be larger than T310)
  mobility_unit_config  mobility_config;
  rrc_appconfig         rrc_config;
  security_unit_config  security_config;
  f1ap_cu_unit_config   f1ap_config;
  cu_cp_log_unit_config loggers;
};

} // namespace srsran
