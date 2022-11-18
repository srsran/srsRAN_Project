/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/carrier_configuration.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/prach/prach_constants.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/sib_configuration.h"
#include "srsgnb/ran/slot_pdu_capacity_contants.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/sr_configuration.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"
#include "srsgnb/scheduler/config/bwp_configuration.h"
#include "srsgnb/scheduler/config/dmrs.h"
#include "srsgnb/scheduler/config/serving_cell_config.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

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
};

/// \c LogicalChannelConfig, TS 38.331.
struct logical_channel_config {
  lcid_t  lcid;
  uint8_t priority;
  // TODO: add remaining fields;
  optional<uint8_t>               lc_group;
  optional<scheduling_request_id> sr_id;
  bool                            lc_sr_mask;
  bool                            lc_sr_delay_timer_applied;
};

/// \remark See TS 38.331, "ServingCellConfig".
struct serving_cell_ue_configuration_request {
  du_cell_index_t cell_index;
  /// \c spCellConfig, as part of \c CellGroupConfig, TS 38.331.
  optional<serving_cell_config> serv_cell_cfg;
};

/// UE Creation Request.
struct sched_ue_creation_request_message {
  du_ue_index_t ue_index;
  rnti_t        crnti;
  /// List of \c mac-LogicalChannelConfig, TS 38.331; contained in \c rlc-BearerToAddModList.
  /// This is common to all cells belonging to \c masterCellGroup, TS 38.331.
  std::vector<logical_channel_config> lc_config_list;
  /// \c schedulingRequestToAddModList, TS 38.331; part of \c schedulingRequestConfig, in \c mac-CellGroupConfig.
  /// This is common to all cells belonging to masterCellGroup, TS 38.331.
  std::vector<scheduling_request_to_addmod> sched_request_config_list;
  /// List of configured cells.
  std::vector<serving_cell_ue_configuration_request> cells;
};

/// UE Reconfiguration Request.
struct sched_ue_reconfiguration_message {
  du_ue_index_t                                   ue_index;
  rnti_t                                          crnti;
  du_cell_index_t                                 pcell_index;
  optional<serving_cell_ue_configuration_request> serv_cell_cfg;
};

/// UE Delete Request.
struct sched_ue_delete_message {
  du_ue_index_t   ue_index;
  rnti_t          crnti;
  du_cell_index_t pcell_index;
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
  virtual void handle_add_ue_request(const sched_ue_creation_request_message& ue_request)            = 0;
  virtual void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) = 0;
  virtual void handle_ue_delete_request(du_ue_index_t ue_index)                                      = 0;
};

/// Interface used by scheduler to notify MAC that a configuration is complete.
class sched_configuration_notifier
{
public:
  virtual ~sched_configuration_notifier()                    = default;
  virtual void on_ue_config_complete(du_ue_index_t ue_index) = 0;
  virtual void on_ue_delete_response(du_ue_index_t ue_index) = 0;
};

} // namespace srsgnb
