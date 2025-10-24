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

/// \file
/// \brief Configuration structs passed to scheduler implementation.

#include "srsran/adt/interval.h"
#include "srsran/ran/direct_current_offset.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/resource_allocation/vrb_to_prb.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/sch/sch_mcs.h"
#include "srsran/ran/sib/sib_configuration.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <chrono>
#include <variant>
#include <vector>

namespace srsran {

/// \brief QoS-aware policy scheduler expert parameters.
struct time_qos_scheduler_expert_config {
  /// \brief Types of scheduler weight functions to use. Supported:
  /// - gbr_prioritized - logical channels with GBR get always prioritized if their BR < GBR.
  /// - multivariate - different weight functions (e.g. GBR, PF) for a given logical channel are "averaged" to obtain
  /// the final weight function. This weight function was taken from B. Bojovic, N. Baldo, “A new Channel and QoS Aware
  /// Scheduler to enhance the capacity of Voice over LTE systems”, in Proceedings of 11th International
  /// Multi-Conference on Systems, Signals & Devices (SSD’14), Castelldefels, 11-14 February 2014,
  /// Castelldefels (Spain).
  enum class weight_function { gbr_prioritized, multivariate };

  /// \brief Determines the scheduler policy weight function to use.
  weight_function qos_weight_func = weight_function::gbr_prioritized;
  /// Fairness Coefficient to use in Proportional Fair weight of the QoS-aware policy.
  double pf_fairness_coeff = 2.0;
  /// Whether to take into account or ignore the QoS Flow priority and ARP priority in the QoS-aware scheduling.
  bool priority_enabled = true;
  /// Whether to take into account or ignore the QoS Flow Packet Delay Budget (PDB) in the QoS-aware scheduling.
  bool pdb_enabled = true;
  /// Whether to take into account or ignore the QoS Flow Guaranteed Bit Rate (GBR) in the QoS-aware scheduling.
  bool gbr_enabled = true;
};

/// \brief Round-Robin policy scheduler expert parameters.
struct time_rr_scheduler_expert_config {};

/// \brief Policy scheduler expert parameters.
using policy_scheduler_expert_config = std::variant<time_qos_scheduler_expert_config, time_rr_scheduler_expert_config>;

struct ul_power_control {
  /// Enable closed-loop PUSCH power control.
  bool enable_pusch_cl_pw_control = false;
  /// Enable bandwidth adaptation to prevent negative PHR.
  bool enable_phr_bw_adaptation = false;
  /// Target PUSCH SINR to be achieved with Close-loop power control, in dB.
  /// Only relevant if \c enable_closed_loop_pw_control is set to true.
  float target_pusch_sinr{10.0f};
  /// Path-loss at which the Target PUSCH SINR is expected to be achieved, in dB.
  /// This is used to compute the path loss compensation for PUSCH fractional power control.
  /// Only relevant if \c enable_closed_loop_pw_control is set to true.
  float path_loss_for_target_pusch_sinr{70.0f};
  /// Enable closed-loop PUCCH power control.
  bool enable_pucch_cl_pw_control = false;
  /// Target PUCCH SINR to be achieved with Close-loop power control, in dB, for the PUCCH formats 0, 2 and 3.
  float pucch_f0_sinr_target_dB = 6.0f;
  float pucch_f2_sinr_target_dB = 3.0f;
  float pucch_f3_sinr_target_dB = -3.0f;
};

/// \brief UE scheduling statically configurable expert parameters.
struct scheduler_ue_expert_config {
  /// Range of allowed MCS indices for DL UE scheduling. To use a fixed mcs, set the minimum mcs equal to the maximum.
  interval<sch_mcs_index, true> dl_mcs{0, 28};
  /// Sequence of redundancy versions used for PDSCH scheduling. Possible values: {0, 1, 2, 3}.
  std::vector<uint8_t> pdsch_rv_sequence = {0};
  /// Range of allowed MCS indices for UL UE scheduling. To use a fixed mcs, set the minimum mcs equal to the maximum.
  interval<sch_mcs_index, true> ul_mcs{0, 28};
  /// Sequence of redundancy versions used for PUSCH scheduling. Possible values: {0, 1, 2, 3}.
  std::vector<uint8_t> pusch_rv_sequence = {0};
  /// CQI used to derived the MCS for DL scheduling, when no CSI has been reported yet.
  unsigned initial_cqi = 3;
  /// Maximum number of DL HARQ retxs.
  unsigned max_nof_dl_harq_retxs = 4;
  /// Maximum number of UL HARQ retxs.
  unsigned max_nof_ul_harq_retxs = 4;
  /// Timeout for DL HARQ with pending retransmission to be discarded.
  std::chrono::milliseconds dl_harq_retx_timeout{100};
  /// Timeout for UL HARQ with pending retransmission to be discarded.
  std::chrono::milliseconds ul_harq_retx_timeout{100};
  /// Maximum MCS index that can be assigned when scheduling MSG4.
  sch_mcs_index max_msg4_mcs = 9;
  /// Initial UL SINR value used for Dynamic UL MCS computation (in dB).
  double initial_ul_sinr = 5.0;
  /// Enable multiplexing of CSI-RS and PDSCH.
  bool enable_csi_rs_pdsch_multiplexing = true;
  /// Set boundaries, in number of RBs, for UE PDSCH grants.
  interval<unsigned> pdsch_nof_rbs{1, MAX_NOF_PRBS};
  /// Set boundaries, in number of RBs, for UE PUSCH grants.
  interval<unsigned> pusch_nof_rbs{1, MAX_NOF_PRBS};
  /// \defgroup ta_manager_params
  /// \brief Time Advance (TA) manager parameters.
  ///
  /// These parameters define the behaviour of the Time Advance manager and on how the Time Advance Command (\f$T_A\f$)
  /// is triggered.
  ///
  /// The TA measurement is reported from the physical layer, averaged over a \ref ta_measurement_slot_period and
  /// outliers are filtered out. The final estimated TA is rounded to the nearest TA unit.
  ///
  /// \remark T_A is defined in TS 38.213, clause 4.2.
  /// @{
  /// Measurements periodicity in nof. slots over which the new Timing Advance Command is computed.
  unsigned ta_measurement_slot_period{80};
  /// \brief Delay in nof. slots between issuing the TA_CMD and starting TA measurements.
  ///
  /// This parameter specifies the mandatory waiting period (i.e. the prohibit period) that must elapse after the
  /// Timing Advance command (TA_CMD) is issued before the system begins its Timing Advance measurements.
  /// The delay allows the system to settle, ensuring that measurements are taken under stable conditions.
  unsigned ta_measurement_slot_prohibit_period{0};
  /// \brief Timing Advance Command (T_A) offset threshold.
  ///
  /// A TA command is triggered if the estimated TA is equal to or greater than this threshold. Possible valid values
  /// are {0,...,32}.
  ///
  /// If set to less than zero, issuing of TA Command is disabled.
  int8_t ta_cmd_offset_threshold = 1;
  /// \brief Timing Advance target in units of TA.
  ///
  /// Offsets the target TA measurements so the signal from the UE is kept delayed. This parameter is useful for
  /// avoiding negative TA when the UE is getting away.
  float ta_target = 0.0F;
  /// UL SINR threshold (in dB) above which reported N_TA update measurement is considered valid.
  float ta_update_measurement_ul_sinr_threshold = 0.0F;
  /// @}
  /// Direct Current (DC) offset, in number of subcarriers, used in PUSCH, by default. The gNB may supersede this DC
  /// offset value through RRC messaging. See TS38.331 - "txDirectCurrentLocation".
  dc_offset_t initial_ul_dc_offset{dc_offset_t::center};
  /// Maximum number of PDSCH grants per slot.
  unsigned max_pdschs_per_slot = MAX_PDSCH_PDUS_PER_SLOT;
  /// Maximum number of PUSCH grants per slot.
  unsigned max_puschs_per_slot = MAX_PUSCH_PDUS_PER_SLOT;
  /// Maximum number of PUCCH grants per slot.
  unsigned max_pucchs_per_slot{31U};
  /// Maximum number of PUSCH + PUCCH grants per slot.
  unsigned max_ul_grants_per_slot{32U};
  /// Possible values: {1, ..., 7}.
  /// [Implementation-defined] As min_k1 is used for both common and dedicated PUCCH configuration, and in the UE
  /// fallback scheduler only allow max k1 = 7, we restrict min_k1 to 7.
  uint8_t min_k1 = 4;
  /// Maximum number of PDCCH grant allocation attempts per slot. Default: Unlimited.
  unsigned max_pdcch_alloc_attempts_per_slot = std::max(MAX_DL_PDCCH_PDUS_PER_SLOT, MAX_UL_PDCCH_PDUS_PER_SLOT);
  /// Offset to apply to the CQI for PDCCH aggregation level calculation.
  float pdcch_al_cqi_offset = 0;
  /// CQI offset increment used in outer loop link adaptation (OLLA) algorithm. If set to zero, OLLA is disabled.
  float olla_cqi_inc{0.001};
  /// DL Target BLER to be achieved with OLLA.
  float olla_dl_target_bler{0.01};
  /// Maximum CQI offset that the OLLA algorithm can apply to the reported CQI.
  float olla_max_cqi_offset{4.0};
  /// UL SNR offset increment in dB used in OLLA algorithm. If set to zero, OLLA is disabled.
  float olla_ul_snr_inc{0.001};
  /// UL Target BLER to be achieved with OLLA.
  float olla_ul_target_bler{0.01};
  /// Maximum UL SNR offset that the OLLA algorithm can apply on top of the estimated UL SINR.
  float olla_max_ul_snr_offset{5.0};
  /// Threshold for drop in CQI of the first HARQ transmission above which HARQ retransmissions are cancelled.
  uint8_t dl_harq_la_cqi_drop_threshold{2};
  /// Threshold for drop in nof. layers of the first HARQ transmission above which HARQ retransmission is cancelled.
  uint8_t dl_harq_la_ri_drop_threshold{1};
  /// Automatic HARQ acknowledgement (used for NTN cases with no HARQ feedback)
  bool auto_ack_harq{false};
  /// Boundaries in RB interval for resource allocation of UE PDSCHs.
  crb_interval pdsch_crb_limits{0, MAX_NOF_PRBS};
  /// Bundle size used for interleaving. Possible values: {0, 2, 4}. When set to 0, interleaving is disabled.
  vrb_to_prb::mapping_type pdsch_interleaving_bundle_size{vrb_to_prb::mapping_type::non_interleaved};
  /// Boundaries in RB interval for resource allocation of UE PUSCHs.
  crb_interval pusch_crb_limits{0, MAX_NOF_PRBS};
  /// Minimum distance between PUCCH and PUSCH in number of PRBs.
  unsigned min_pucch_pusch_prb_distance = 1;
  /// Expert parameters to be passed to the policy scheduler.
  policy_scheduler_expert_config strategy_cfg = time_qos_scheduler_expert_config{};
  /// \brief Size of the group of UEs that is considered for newTx allocation in a given slot. The groups of UEs
  /// will rotate in a round-robin fashion.
  /// To minimize computation load, a lower group size can be used. If the QoS scheduler policy is used, this will
  /// mean that the QoS priority is only computed to a subset of UEs and the scheduler will operate like a hybrid of
  /// round-robin and QoS.
  unsigned pre_policy_rr_ue_group_size = 32;
  /// Expert PUCCH/PUSCH power control parameters.
  ul_power_control ul_power_ctrl = ul_power_control{};
};

/// \brief System Information scheduling statically configurable expert parameters.
struct scheduler_si_expert_config {
  /// As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for SIB1.
  sch_mcs_index     sib1_mcs_index          = 5;
  aggregation_level sib1_dci_aggr_lev       = aggregation_level::n4;
  sch_mcs_index     si_message_mcs_index    = 5;
  aggregation_level si_message_dci_aggr_lev = aggregation_level::n4;
  /// SIB1 retx period.
  sib1_rtx_periodicity sib1_retx_period = sib1_rtx_periodicity::ms160;
};

/// \brief Random Access scheduling statically configurable expert parameters.
struct scheduler_ra_expert_config {
  /// MCS to use for RAR PDSCH.
  sch_mcs_index rar_mcs_index = 0;
  /// MCS to use for Msg3 PUSCH.
  sch_mcs_index msg3_mcs_index = 0;
  /// Maximum number of Msg3 PUSCH retransmissions.
  unsigned max_nof_msg3_harq_retxs = 4;
  /// Timeout for Msg3 HARQ with pending retransmission to be discarded.
  std::chrono::milliseconds harq_retx_timeout{100};
};

/// \brief Paging scheduling statically configurable expert parameters.
struct scheduler_paging_expert_config {
  /// As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for Paging.
  sch_mcs_index     paging_mcs_index    = 5;
  aggregation_level paging_dci_aggr_lev = aggregation_level::n4;
  unsigned          max_paging_retries  = 2;
};

/// \brief Scheduling statically configurable expert parameters.
struct scheduler_expert_config {
  scheduler_si_expert_config     si;
  scheduler_ra_expert_config     ra;
  scheduler_paging_expert_config pg;
  scheduler_ue_expert_config     ue;
  bool                           log_broadcast_messages       = false;
  bool                           log_high_latency_diagnostics = false;
};

} // namespace srsran
