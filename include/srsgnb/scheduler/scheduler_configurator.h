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

#include "bwp_configuration.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/carrier_configuration.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/prach/prach_constants.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/sib_configuration.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"
#include "srsgnb/scheduler/dmrs.h"
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

  /// [Implementation-defined] SIB1 parameters.
  sib1_rtx_periodicity sib1_retx_period;
  uint8_t              sib1_mcs;
  uint8_t              sib1_rv;
  aggregation_level    sib1_dci_aggr_lev;
  /// Payload size is in bytes.
  unsigned sib1_payload_size;
};

/// \remark See TS 38.331, "PDCCH-Config"
struct pdcch_config {
  /// List of CORESETs to be used by the UE. In case of CORESET Id overlap with commonControlResourceSet,
  /// the CORESET in this list takes precedence. Size: (0..3).
  std::vector<coreset_configuration>      coreset_to_addmod_list;
  std::vector<coreset_id>                 coreset_to_rel_list;
  std::vector<search_space_configuration> ss_to_addmod_list;
  std::vector<search_space_id>            ss_to_rel_list;
  // TODO: add remaining fields.
};

/// UE-dedicated PDSCH Configuration as per TS38.331, "PDSCH-Config".
struct pdsch_config {
  /// Identifier used to initialize data scrambling (c_init) for PDSCH. If the field is absent, the UE applies the PCI.
  /// See TS38.331, \e dataScramblingIdentityPDSCH, and TS38.211, 7.3.1.1. Values: {0,...,1023}.
  optional<uint16_t> data_scrambling_id_pdsch;
  /// PDSCH time domain resource allocations. Size: (0..maxNrofDL-Allocations=16).
  std::vector<pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;
  // TODO: Remaining.
};

/// \remark See TS 38.331, "BWP-DownlinkDedicated"
struct bwp_downlink_dedicated {
  optional<pdcch_config> pdcch_cfg;
  optional<pdsch_config> pdsch_cfg;
  // TODO: Remaining
};

/// \remark TS 38.331, "BWP-Downlink".
struct bwp_downlink {
  bwp_id_t                         bwp_id;
  optional<bwp_downlink_common>    bwp_dl_common;
  optional<bwp_downlink_dedicated> bwp_dl_ded;
};

/// \remark See TS 38.331, "ServingCellConfig"
struct serving_cell_ue_configuration_request {
  /// Initial Downlink BWP.
  optional<bwp_downlink_dedicated> init_dl_bwp;
  /// List of UE DL BWPs. Size: (0..maxNrofBWPs=4)
  std::vector<bwp_downlink> dl_bwps_to_addmod_list;
  /// List of UE DL BWPs to remove. Size: (0..maxNrofBWPs=4)
  std::vector<bwp_id_t> dl_bwps_to_rel_list;
};

/// UE Creation Request.
struct sched_ue_creation_request_message {
  du_ue_index_t                         ue_index;
  rnti_t                                crnti;
  du_cell_index_t                       pcell_index;
  serving_cell_ue_configuration_request serv_cell_cfg;
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
    unsigned                                                    start_symbol;
    unsigned                                                    frequency_index;
    static_vector<preamble, prach_constants::MAX_NUM_PREAMBLES> preambles;
  };

  static_vector<occasion, prach_constants::MAX_PRACH_OCCASIONS> occasions;
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
