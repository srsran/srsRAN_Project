/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sib_configuration.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/ran/ssb_configuration.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/tdd_ul_dl_config.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/dmrs.h"
#include "srsran/scheduler/config/logical_channel_config.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {

/// Basic scheduler resource grid element for resource reservation.
struct sched_grid_resource {
  prb_interval      prbs;
  ofdm_symbol_range symbols;

  bool operator==(const sched_grid_resource& rhs) const { return prbs == rhs.prbs and symbols == rhs.symbols; }

  bool operator!=(const sched_grid_resource& rhs) const { return !(rhs == *this); }

  bool is_empty() const { return prbs.empty() and symbols.empty(); }
};

/// Cell Configuration Request.
/// \remark See O-RAN WG8, Section 9.2.3.2.1, Table 9.18.
struct sched_cell_configuration_request_message {
  du_cell_index_t cell_index;
  uint8_t         nof_beams;     // (0..64)
  uint8_t         nof_layers;    // (0..8)
  uint8_t         nof_ant_ports; // (0..64)
  pci_t           pci;

  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;

  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;

  /// Imported from mac_cell_configuration (NR Cell Configuration, O-RAN WG8, Section 9.2.1.1).
  subcarrier_spacing    scs_common;
  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;
  ssb_configuration     ssb_config;
  dmrs_typeA_position   dmrs_typeA_pos;
  /// This corresponds to controlResourceSetZero as per PDCCH-ConfigSIB1, TS 38.331.
  uint8_t coreset0;
  /// This corresponds to searchSpaceZero as per PDCCH-ConfigSIB1, TS 38.331.
  uint8_t searchspace0;

  /// Payload size is in bytes.
  unsigned sib1_payload_size;

  /// List of PUCCH guardbands;
  std::vector<sched_grid_resource> pucch_guardbands;

  /// CSI-RS scheduling parameters.
  optional<csi_meas_config> csi_meas_cfg;
};

/// Request for a new UE configuration provided to the scheduler during UE creation or reconfiguration.
struct sched_ue_config_request {
  /// List of configured Logical Channels. See \c mac-LogicalChannelConfig, TS38.331.
  std::vector<logical_channel_config> lc_config_list;
  /// List of configured Scheduling Request resources. See \c schedulingRequestConfig, TS38.331.
  std::vector<scheduling_request_to_addmod> sched_request_config_list;
  /// UE-dedicated configuration for the PCell and SCells.
  std::vector<cell_config_dedicated> cells;
};

/// UE Creation Request.
struct sched_ue_creation_request_message {
  du_ue_index_t           ue_index;
  rnti_t                  crnti;
  sched_ue_config_request cfg;
};

/// UE Reconfiguration Request.
struct sched_ue_reconfiguration_message {
  du_ue_index_t           ue_index;
  rnti_t                  crnti;
  sched_ue_config_request cfg;
};

/// UE Delete Request.
struct sched_ue_delete_message {
  du_ue_index_t ue_index;
  rnti_t        crnti;
};

/// RACH indication Message. It contains all the RACHs detected in a given slot and cell.
struct rach_indication_message {
  du_cell_index_t cell_index;
  slot_point      slot_rx;

  struct preamble {
    unsigned      preamble_id;
    rnti_t        tc_rnti;
    phy_time_unit time_advance;
  };

  struct occasion {
    /// Index of the first OFDM Symbol where RACH was detected.
    unsigned                                                  start_symbol;
    unsigned                                                  frequency_index;
    static_vector<preamble, MAX_PREAMBLES_PER_PRACH_OCCASION> preambles;
  };

  static_vector<occasion, MAX_PRACH_OCCASIONS_PER_SLOT> occasions;
};

/// Interface to Add/Remove UEs and Cells.
class scheduler_configurator
{
public:
  virtual ~scheduler_configurator()                                                                   = default;
  virtual bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) = 0;
  virtual void handle_rach_indication(const rach_indication_message& msg)                             = 0;
};

class scheduler_ue_configurator
{
public:
  virtual ~scheduler_ue_configurator()                                                               = default;
  virtual void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request)       = 0;
  virtual void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) = 0;
  virtual void handle_ue_removal_request(du_ue_index_t ue_index)                                     = 0;
};

/// Interface used by scheduler to notify MAC that a configuration is complete.
class sched_configuration_notifier
{
public:
  virtual ~sched_configuration_notifier()                    = default;
  virtual void on_ue_config_complete(du_ue_index_t ue_index) = 0;
  virtual void on_ue_delete_response(du_ue_index_t ue_index) = 0;
};

} // namespace srsran
