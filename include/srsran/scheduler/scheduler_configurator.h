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

#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/drx_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/meas_gap_config.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/rrm.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/ran/ssb/ssb_configuration.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/ran/time_alignment_config.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/dmrs.h"
#include "srsran/scheduler/config/logical_channel_config.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/config/si_scheduling_config.h"
#include "srsran/scheduler/config/slice_rrm_policy_config.h"

namespace srsran {

class scheduler_cell_metrics_notifier;

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
  struct metrics_config {
    std::chrono::milliseconds        report_period{0};
    scheduler_cell_metrics_notifier* notifier = nullptr;
    /// Maximum number of UE events per report.
    unsigned max_ue_events_per_report = 64;
  };

  du_cell_index_t       cell_index;
  du_cell_group_index_t cell_group_index;
  uint8_t               nof_beams; // (0..64)
  pci_t                 pci;

  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;

  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;

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
  units::bytes sib1_payload_size;

  /// Scheduling of SI messages.
  std::optional<si_scheduling_config> si_scheduling;

  /// List of PUCCH guardbands.
  std::vector<sched_grid_resource> pucch_guardbands;

  /// List of zp-CSI-RS resources common to all UEs.
  std::vector<zp_csi_rs_resource> zp_csi_rs_list;

  /// List of nzp-CSI-RS resources common to all UEs.
  std::vector<nzp_csi_rs_resource> nzp_csi_rs_res_list;

  /// List of dl-DataToUL-ACK values sent to UE in its dedicated configuration.
  static_vector<uint8_t, 8> dl_data_to_ul_ack;

  /// List of RAN slices to support in the scheduler.
  std::vector<slice_rrm_policy_config> rrm_policy_members;

  unsigned ntn_cs_koffset = 0;

  bool cfra_enabled = false;

  /// Configuration of scheduler cell metrics.
  metrics_config metrics;
};

/// Cell Reconfiguration Request.
struct sched_cell_reconfiguration_request_message {
  std::optional<du_cell_slice_reconfig_request> slice_reconf_req;
};

/// Parameters provided to the scheduler to configure the resource allocation of a specific UE.
struct sched_ue_resource_alloc_config {
  /// Minimum and maximum PDSCH grant sizes for the given UE.
  prb_interval pdsch_grant_size_limits{0, MAX_NOF_PRBS};
  /// Minimum and maximum PUSCH grant sizes for the given UE.
  prb_interval pusch_grant_size_limits{0, MAX_NOF_PRBS};
  /// Maximum PDSCH HARQ retransmissions.
  unsigned max_pdsch_harq_retxs = 4;
  /// Maximum PUSCH HARQ retransmissions.
  unsigned max_pusch_harq_retxs = 4;
  // RRM policy for the UE.
  rrm_policy_ratio_group rrm_policy_group;
};

/// Request for a new UE configuration provided to the scheduler during UE creation or reconfiguration.
struct sched_ue_config_request {
  /// List of configured Logical Channels. See \c mac-LogicalChannelConfig, TS38.331.
  std::optional<std::vector<logical_channel_config>> lc_config_list;
  /// List of configured Scheduling Request resources. See \c schedulingRequestConfig, TS38.331.
  std::optional<std::vector<scheduling_request_to_addmod>> sched_request_config_list;
  /// UE-dedicated configuration for the PCell and SCells.
  std::optional<std::vector<cell_config_dedicated>> cells;
  /// Resource allocation configuration for the given UE.
  std::optional<sched_ue_resource_alloc_config> res_alloc_cfg;
  /// DRX-Config.
  std::optional<drx_config> drx_cfg;
  /// measGapConfig.
  std::optional<meas_gap_config> meas_gap_cfg;
  /// Whether this configuration procedure comes after rrcReestablishment.
  bool reestablished;
};

/// Request to create a new UE in scheduler.
struct sched_ue_creation_request_message {
  du_ue_index_t ue_index;
  rnti_t        crnti;
  /// Whether the UE starts in fallback mode, i.e. without using its dedicated configuration.
  bool starts_in_fallback;
  /// Slot at which UL-CCCH message was received, in case of RA-based UE creation. Invalid, otherwise.
  std::optional<slot_point> ul_ccch_slot_rx;
  /// Configuration to be applied to the new UE.
  sched_ue_config_request cfg;
  /// Time Alignment Group configuration.
  static_vector<time_alignment_group, MAX_NOF_TIME_ALIGNMENT_GROUPS> tag_config;
};

/// UE Reconfiguration Request.
struct sched_ue_reconfiguration_message {
  du_ue_index_t           ue_index;
  rnti_t                  crnti;
  sched_ue_config_request cfg;
  bool                    reestablished;
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
    unsigned preamble_id;
    /// Allocated TC-RNTI, for Contention-based RACH, or C-RNTI, for Contention-free RACH.
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
  virtual ~scheduler_configurator() = default;

  /// \brief Reconfigure cell.
  ///
  /// This method cannot be called for an existing cell index without first removing it.
  virtual bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) = 0;

  /// \brief Handle cell configuration removal.
  virtual void handle_cell_removal_request(du_cell_index_t cell_index) = 0;

  /// \brief Handle RACH indication message.
  virtual void handle_rach_indication(const rach_indication_message& msg) = 0;

  /// \brief Activate a configured cell. This method has no effect if the cell is already active.
  /// \remark This method needs to be called in the same thread as the slot_indication() method.
  virtual void handle_cell_activation_request(du_cell_index_t cell_index) = 0;

  /// \brief Deactivate a configured cell. This method has no effect if the cell is already deactivated.
  /// \remark This method needs to be called after the last slot_indication() call.
  virtual void handle_cell_deactivation_request(du_cell_index_t cell_index) = 0;

  /// \brief Handle slice reconfiguration request of a cell.
  virtual void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& msg) = 0;
};

class scheduler_ue_configurator
{
public:
  virtual ~scheduler_ue_configurator()                                                               = default;
  virtual void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request)       = 0;
  virtual void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) = 0;
  virtual void handle_ue_removal_request(du_ue_index_t ue_index)                                     = 0;
  virtual void handle_ue_config_applied(du_ue_index_t ue_index)                                      = 0;
};

/// Interface used by scheduler to notify MAC that a configuration is complete.
class sched_configuration_notifier
{
public:
  virtual ~sched_configuration_notifier() = default;

  /// Called by scheduler when UE creation/modification is completed.
  virtual void on_ue_config_complete(du_ue_index_t ue_index, bool ue_creation_result) = 0;

  /// Called by scheduler when UE removal is completed.
  virtual void on_ue_deletion_completed(du_ue_index_t ue_index) = 0;
};

} // namespace srsran
