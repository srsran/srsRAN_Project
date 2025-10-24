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

#include "apps/helpers/metrics/metrics_config.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/direct_current_offset.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/ntn.h"
#include "srsran/ran/pcch/pcch_configuration.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/qos/five_qi.h"
#include "srsran/ran/radio_link_monitoring.h"
#include "srsran/ran/rb_id.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sib/system_info_config.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/tac.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/srslog/srslog.h"
#include <map>
#include <string>
#include <vector>

namespace srsran {

/// DU high logging functionalities.
struct du_high_unit_logger_config {
  srslog::basic_levels du_level   = srslog::basic_levels::warning;
  srslog::basic_levels mac_level  = srslog::basic_levels::warning;
  srslog::basic_levels rlc_level  = srslog::basic_levels::warning;
  srslog::basic_levels f1ap_level = srslog::basic_levels::warning;
  srslog::basic_levels f1u_level  = srslog::basic_levels::warning;
  srslog::basic_levels gtpu_level = srslog::basic_levels::warning;

  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to true to log broadcasting messages and all PRACH opportunities.
  bool broadcast_enabled = false;
  /// Enable JSON generation for the F1AP Tx and Rx PDUs.
  bool f1ap_json_enabled = false;
  /// Log metrics (e.g. context switches) when RT slowdowns are detected.
  bool high_latency_diagnostics_enabled = false;
};

/// DU high tracing functionalities.
struct du_high_unit_tracer_config {
  /// \brief Whether to enable tracing of the DU-high executors.
  bool executor_tracing_enable = false;
};

/// Timing Advance MAC CE scheduling expert configuration.
struct du_high_unit_ta_sched_expert_config {
  /// Measurements periodicity in nof. slots over which the new Timing Advance Command is computed.
  unsigned ta_measurement_slot_period = 80;
  ///  Delay in nof. slots between issuing the TA_CMD and starting TA measurements.
  unsigned ta_measurement_slot_prohibit_period = 0;
  /// Timing Advance Command (T_A) offset threshold above which Timing Advance Command is triggered. Possible valid
  /// values {0,...,32}. If set to less than zero, issuing of TA Command is disabled.
  /// \remark T_A is defined in TS 38.213, clause 4.2.
  int ta_cmd_offset_threshold = 1;
  /// Timing Advance target in units of TA.
  float ta_target = 1.0F;
  /// UL SINR threshold (in dB) above which reported N_TA update measurement is considered valid.
  float ta_update_measurement_ul_sinr_threshold = 0.0F;
};

/// Scheduler expert configuration.
struct du_high_unit_scheduler_expert_config {
  /// \brief Number of UEs pre-selected for PDSCH/PUSCH newTx scheduling in each slot. The scheduling policy will only
  /// be applied to the pre-selected UEs.
  ///
  /// When \c nof_preselected_newtx_ues is lower than the total number of active UEs in the cell, the scheduler will
  /// operate as a hybrid between a Round-Robin scheduler and whichever scheduler policy was selected (e.g. QoS-aware).
  /// This parameter is useful to reduce the complexity of the scheduling decision, at the cost of some performance.
  unsigned nof_preselected_newtx_ues = 32;
  /// Policy scheduler expert parameters.
  std::optional<policy_scheduler_expert_config> policy_sched_expert_cfg;
  /// Timing Advance MAC CE scheduling expert configuration.
  du_high_unit_ta_sched_expert_config ta_sched_cfg;
};

/// DRX configuration.
struct du_high_unit_drx_config {
  /// drx-onDurationTimer in milliseconds, as per TS 38.331.
  unsigned on_duration_timer = 10;
  /// drx-InactivityTimer in milliseconds, as per TS 38.331.
  unsigned inactivity_timer = 0;
  /// drx-RetransmissionTimerDL in slots, as per TS 38.331.
  unsigned retx_timer_dl = 0;
  /// drx-RetransmissionTimerUL in slots, as per TS 38.331.
  unsigned retx_timer_ul = 0;
  /// drx-LongCycle in milliseconds, as per TS 38.331. The value 0 is used for disabling DRX.
  unsigned long_cycle = 0;
};

struct du_high_unit_ssb_config {
  /// SSB period in milliseconds.
  unsigned ssb_period_msec = 10;
  /// \brief \c ss-PBCH-BlockPower, part of \c ServingCellConfigCommonSIB, as per TS 38.331.
  /// Average EPRE of the REs that carry secondary synchronization signals in dBm used for SSB transmission.
  /// Values: {-60,..,70}
  int ssb_block_power = -16;
  /// PSS EPRE to SSS EPRE for SSB, as per TS 38.213, Section 4.1.
  ssb_pss_to_sss_epre pss_to_sss_epre = ssb_pss_to_sss_epre::dB_0;
};

/// Common uplink parameters of a cell.
struct du_high_unit_ul_common_config {
  /// Maximum transmit power allowed in this serving cell. Values: {-30,...,33}dBm.
  std::optional<int> p_max;
  /// Maximum number of PUCCH grants per slot.
  unsigned max_pucchs_per_slot = 31U;
  /// Maximum number of PUSCH + PUCCH grants per slot.
  unsigned max_ul_grants_per_slot = 32U;
  /// Minimum distance in PRBs between PUCCH and UE-dedicated PUSCH grants.
  unsigned min_pucch_pusch_prb_distance = 1U;
};

/// PDSCH application configuration.
struct du_high_unit_pdsch_config {
  /// Minimum modulation and coding scheme index for C-RNTI PDSCH allocations. Note that setting a high minimum MCS
  /// may lead to a high BLER if the SINR is low.
  unsigned min_ue_mcs = 0;
  /// Maximum modulation and coding scheme index for C-RNTI PDSCH allocations. To set a fixed MCS, set \c min_ue_mcs
  /// equal to the \c max_ue_mcs.
  unsigned max_ue_mcs = 28;
  /// RAR modulation and coding scheme index.
  unsigned fixed_rar_mcs = 0;
  /// SI modulation and coding scheme index.
  unsigned fixed_sib1_mcs = 5;
  /// Set DL HARQ Mode B in NTN cell.
  /// HARQ mode B allows scheduling and reusing a HARQ process before the RTT of the previous HARQ process has elapsed.
  bool harq_mode_b = false;
  /// Number of UE DL HARQ processes.
  unsigned nof_harqs = 16;
  /// Maximum number of times a DL HARQ process can be retransmitted, before it gets discarded.
  unsigned max_nof_harq_retxs = 4;
  /// \brief Maximum time, in milliseconds, between a HARQ NACK and the scheduler allocating the respective HARQ for
  /// retransmission. If this timeout is exceeded, the HARQ process is discarded.
  unsigned harq_retx_timeout = 100;
  /// Maximum number of consecutive DL KOs before an RLF is reported.
  unsigned max_consecutive_kos = 100;
  /// Redundancy version sequence to use. Each element can have one of the following values: {0, 1, 2, 3}.
  std::vector<unsigned> rv_sequence = {0, 2, 3, 1};
  /// MCS table to use for PDSCH
  pdsch_mcs_table mcs_table = pdsch_mcs_table::qam256;
  /// Minimum number of RBs for resource allocation of UE PDSCHs.
  unsigned min_rb_size = 1;
  /// Maximum number of RBs for resource allocation of UE PDSCHs.
  unsigned max_rb_size = MAX_NOF_PRBS;
  /// Start RB for resource allocation of UE PDSCHs.
  unsigned start_rb = 0;
  /// End RB for resource allocation of UE PDSCHs.
  unsigned end_rb = MAX_NOF_PRBS;
  /// Maximum number of PDSCH grants per slot.
  unsigned max_pdschs_per_slot = MAX_PDSCH_PDUS_PER_SLOT;
  /// Maximum number of DL or UL PDCCH allocation attempts per slot.
  unsigned max_pdcch_alloc_attempts_per_slot = std::max(MAX_DL_PDCCH_PDUS_PER_SLOT, MAX_UL_PDCCH_PDUS_PER_SLOT);
  /// CQI offset increment used in outer loop link adaptation (OLLA) algorithm. If set to zero, OLLA is disabled.
  float olla_cqi_inc{0.001};
  /// DL Target BLER to be achieved with OLLA.
  float olla_target_bler{0.01};
  /// Maximum CQI offset that the OLLA algorithm can apply to the reported CQI.
  float olla_max_cqi_offset{4.0};
  /// Direct Current (DC) offset, in number of subcarriers, used to populate \c txDirectCurrentLocation-v1530 in SIB1
  /// under FrequencyInfoDL IE.
  ///
  /// The numerology of the active DL BWP is used as a reference to determine the number of subcarriers.
  /// The DC offset value 0 corresponds to the center of the SCS-Carrier for the numerology of the active DL BWP.
  dc_offset_t dc_offset{dc_offset_t::center};
  /// Link Adaptation (LA) threshold for drop in CQI of the first HARQ transmission above which HARQ retransmissions
  /// are cancelled.
  uint8_t harq_la_cqi_drop_threshold{3};
  /// Link Adaptation (LA) threshold for drop in nof. layers of the first HARQ transmission above which HARQ
  /// retransmission is cancelled.
  uint8_t harq_la_ri_drop_threshold{1};
  /// Position for additional DM-RS in DL, see Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 in TS 38.211.
  unsigned dmrs_add_pos{2};
  /// \brief Bundle size used for interleaving.
  ///
  /// Controls the bundle size used for interleaving for PDSCH transmissions scheduled on dedicated search spaces. If
  /// set to zero, interleaving will be disabled. All other PDSCH transmissions will be always non-interleaved.
  vrb_to_prb::mapping_type interleaving_bundle_size{vrb_to_prb::mapping_type::non_interleaved};
  /// Limits the maximum rank UEs can report. It must not exceed the number of transmit antennas.
  std::optional<unsigned> max_rank;
  /// Enable multiplexing of CSI-RS and PDSCH.
  bool enable_csi_rs_pdsch_multiplexing = true;
};

/// PUSCH application configuration.
struct du_high_unit_pusch_config {
  /// \brief Minimum modulation and coding scheme index for C-RNTI PUSCH allocations. Note that setting a high minimum
  /// MCS may lead to a high BLER if the SINR is low.
  unsigned min_ue_mcs = 0;
  /// Maximum modulation and coding scheme index for C-RNTI PUSCH allocations. To set a fixed MCS, set \c min_ue_mcs
  /// equal to the \c max_ue_mcs.
  unsigned max_ue_mcs = 28;
  /// Set UL HARQ Mode B in NTN cell.
  /// HARQ mode B allows scheduling and reusing a HARQ process before the RTT of the previous HARQ process has elapsed.
  bool harq_mode_b = false;
  /// Number of UE UL HARQ processes.
  unsigned nof_harqs = 16;
  /// Maximum number of times a UL HARQ process can be retransmitted, before it gets discarded.
  unsigned max_nof_harq_retxs = 4;
  /// \brief Maximum time, in milliseconds, between a CRC=KO and the scheduler allocating the respective HARQ for
  /// retransmission. If this timeout is exceeded, the HARQ process is discarded.
  unsigned harq_retx_timeout = 100;
  /// Maximum number of consecutive UL KOs before an RLF is reported.
  unsigned max_consecutive_kos = 100;
  /// Redundancy version sequence to use. Each element can have one of the following values: {0, 1, 2, 3}.
  std::vector<unsigned> rv_sequence = {0};
  /// Maximum rank. Limits the number of layers for PUSCH transmissions.
  unsigned max_rank = 4;
  /// MCS table to use for PUSCH
  pusch_mcs_table mcs_table = pusch_mcs_table::qam256;
  /// \c msg3-DeltaPreamble, TS 38.331. Values: {-1,...,6}.
  int msg3_delta_preamble = 6;
  /// \c p0-NominalWithGrant, TS 38.331. Value in dBm. Only even values allowed within {-202,...,24}.
  int p0_nominal_with_grant = -76;

  /// \defgroup betaoffsets
  /// \brief Beta offsets for uplink control information multiplexed in PUSCH.
  ///
  /// Default beta offset values have been derived from the formula
  /// \f$\beta^{PUSCH}_{\text{offset}}\gt \frac{E^{O_{UCI}}_{min}+K_{OH}Q_m\nu}{O_{UCI}+L_{UCI}}R\f$ where:
  /// - \f$E^{O_{UCI}}_{min}\f$ is the minimum number of rate matched bits that guarantee the detection of the UCI bits;
  /// - \f$K_{OH}\f$ is the number of overhead resource elements (i.e., DC position);
  /// - \f$Q_m\f$ is the maximum modulation order, fixed to \f$8\f$;
  /// - \f$\nu\f$ is the maximum number of layers, fixed to \f$2\f$;
  /// - \f$R\f$ is the maximum code rate, fixed to \f$948/1024\f$;
  /// - \f$O_{UCI}\f$ is the number of information bits; and
  /// - \f$L_{UCI}\f$ is the number of CRC bits.
  ///
  /// @{
  /// \brief Parameter \e betaOffsetACK-Index1, in Information Element \c BetaOffsets, TS38.331. Values: {0,...,15}.
  ///
  /// Used for deriving \f$\beta_{\text{HARQ−ACK}}^{offset}\f$ when the PUSCH multiplexes up two HARQ-ACK information
  /// bits. The default value assumes \f$E^{O_{UCI}}_{min}=24\f$ and \f$K_{OH}=1\f$.
  unsigned beta_offset_ack_idx_1 = 11;
  /// \brief Parameter \e betaOffsetACK-Index2, in Information Element \c BetaOffsets, TS38.331. Values: {0,...,15}.
  ///
  /// Used for deriving \f$\beta_{\text{HARQ−ACK}}^{offset}\f$ when the PUSCH multiplexes up 11 HARQ-ACK information
  /// bits. The default value assumes \f$E^{O_{UCI}}_{min}=32\f$ and \f$K_{OH}=2\f$.
  unsigned beta_offset_ack_idx_2 = 6;
  /// \brief Parameter \e betaOffsetACK-Index3, in Information Element \c BetaOffsets, TS38.331. Values: {0,...,15}.
  ///
  /// Used for deriving \f$\beta_{\text{HARQ−ACK}}^{offset}\f$ when the PUSCH multiplexes more than 11 HARQ-ACK
  /// information bits. The default value assumes \f$E^{O_{UCI}}_{min}=40\f$ and \f$K_{OH}=2\f$.
  unsigned beta_offset_ack_idx_3 = 4;
  /// \brief Parameter \e betaOffsetCSI-Part1-Index1, in Information Element \c BetaOffsets, TS38.331. Values:
  /// {0,...,18}.
  ///
  /// Used for deriving \f$\beta_{\text{CSI−1}}^{offset}\f$ when the PUSCH multiplexes up to 11 CSI Part 1 information
  /// bits. The default value assumes \f$E^{O_{UCI}}_{min}=32\f$ and \f$K_{OH}=2\f$.
  unsigned beta_offset_csi_p1_idx_1 = 13;
  /// \brief Parameter \e betaOffsetCSI-Part1-Index2, in Information Element \c BetaOffsets, TS38.331. Values:
  /// {0,...,18}.
  ///
  /// Used for deriving \f$\beta_{\text{CSI−1}}^{offset}\f$ when the PUSCH multiplexes more than 11 CSI Part 1
  /// information bits. The default value assumes \f$E^{O_{UCI}}_{min}=40\f$ and \f$K_{OH}=2\f$.
  unsigned beta_offset_csi_p1_idx_2 = 10;
  /// \brief Parameter \e betaOffsetCSI-Part2-Index1, in Information Element \c BetaOffsets, TS38.331. Values:
  /// {0,...,18}.
  ///
  /// Used for deriving \f$\beta_{\text{CSI−2}}^{offset}\f$ when the PUSCH multiplexes up to 11 CSI Part 2 information
  /// bits. The default value assumes \f$E^{O_{UCI}}_{min}=32\f$ and \f$K_{OH}=2\f$.
  unsigned beta_offset_csi_p2_idx_1 = 13;
  /// \brief Parameter \e betaOffsetCSI-Part2-Index2, in Information Element \c BetaOffsets, TS38.331. Values:
  /// {0,...,18}.
  ///
  /// Used for deriving \f$\beta_{\text{CSI−2}}^{offset}\f$ when the PUSCH multiplexes more than 11 CSI Part 2
  /// information bits. The default value assumes \f$E^{O_{UCI}}_{min}=40\f$ and \f$K_{OH}=2\f$.
  unsigned beta_offset_csi_p2_idx_2 = 10;
  /// @}

  /// \brief Power level corresponding to MSG-3 TPC command in dB, as per Table 8.2-2, TS38.213.
  /// Values {-6,...,8} and must be a multiple of 2.
  int msg3_delta_power = 8;

  /// Minimum k2 value (distance in slots between UL PDCCH and PUSCH) that the gNB can use. Values: {1, ..., 32}.
  unsigned min_k2 = 4;
  /// Maximum number of PUSCH grants per slot.
  unsigned max_puschs_per_slot = MAX_PUSCH_PDUS_PER_SLOT;
  /// \brief Direct Current (DC) offset, in number of subcarriers, used in PUSCH.
  ///
  /// The numerology of the active UL BWP is used as a reference to determine the number of subcarriers.
  /// The DC offset value 0 corresponds to the center of the SCS-Carrier for the numerology of the active UL BWP.
  dc_offset_t dc_offset = dc_offset_t::center;

  /// UL SNR offset increment used in outer loop link adaptation (OLLA) algorithm. If set to zero, OLLA is disabled.
  float olla_snr_inc{0.001};
  /// UL Target BLER to be achieved with OLLA.
  float olla_target_bler{0.01};
  /// Maximum CQI offset that the OLLA algorithm can apply to the reported CQI.
  float olla_max_snr_offset{5.0};
  /// Position for additional DM-RS in UL (see TS 38.211, clause 6.4.1.1.3).
  unsigned dmrs_add_pos{2};
  /// Minimum number of RBs for resource allocation of UE PUSCHs.
  unsigned min_rb_size = 1;
  /// Maximum number of RBs for resource allocation of UE PUSCHs.
  unsigned max_rb_size = MAX_NOF_PRBS;
  /// Start RB for resource allocation of UE PUSCHs.
  unsigned start_rb = 0;
  /// End RB for resource allocation of UE PUSCHs.
  unsigned end_rb = MAX_NOF_PRBS;

  /// Enable closed-loop PUSCH power control.
  bool enable_closed_loop_pw_control = false;
  /// Enable bandwidth adaptation to prevent negative PHR.
  bool enable_phr_bw_adaptation = false;
  /// Target PUSCH SINR to be achieved with close-loop power control, in dB. Only relevant if \c
  /// enable_closed_loop_pw_control is set to true.
  float target_pusch_sinr{10.0f};
  /// Path-loss at which the Target PUSCH SINR is expected to be achieved, in dB.
  /// This is used to compute the path loss compensation for PUSCH fractional power control. The value must be positive.
  /// Only relevant if \c enable_closed_loop_pw_control is set to true and \c path_loss_compensation_factor is set to a
  /// value different from 1.0.
  float path_loss_for_target_pusch_sinr{70.0f};
  /// Factor "alpha" for fractional path-loss compensation in PUSCH power control.
  /// Values: {0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}.
  float path_loss_compensation_factor{1.0f};

  /// Set to true to enable transform precoding in PUSCH.
  bool enable_transform_precoding = false;
};

struct du_high_unit_pucch_config {
  /// \c PUCCH-ConfigCommon parameters.
  /// \c p0-nominal, TS 38.331. Value in dBm. Only even values allowed within {-202,...,24}.
  int p0_nominal = -90;
  /// \c pucch-ResourceCommon, TS 38.331. Values: {0,...,15}. Defines the PUCCH resource set used common configuration.
  std::optional<unsigned> pucch_resource_common;

  /// \c PUCCH-Config parameters.
  /// Force Format 0 for the PUCCH resources belonging to PUCCH resource set 0.
  bool use_format_0 = false;
  /// Select the format for the PUCCH resources belonging to PUCCH resource set 1. Values: {2, 3, 4}.
  pucch_format set1_format = pucch_format::FORMAT_2;
  /// Number of PUCCH resources per UE (per PUCCH resource set) for HARQ-ACK reporting.
  /// Values {3,...,8} if \c use_format_0 is set. Else, Values {1,...,8}.
  /// \remark We assume the number of PUCCH F0/F1 resources for HARQ-ACK is equal to the equivalent number of Format 2
  /// resources.
  unsigned nof_ue_pucch_res_harq_per_set = 8;
  /// \brief Number of separate PUCCH resource sets for HARQ-ACK reporting that are available in a cell.
  /// \remark UEs will be distributed possibly over different HARQ-ACK PUCCH sets; the more sets, the fewer UEs will
  /// have to share the same set, which reduces the chances that UEs won't be allocated PUCCH due to lack of
  /// resources. However, the usage of PUCCH-dedicated REs will be proportional to the number of sets.
  unsigned nof_cell_harq_pucch_sets = 2;
  /// Number of PUCCH Format 1 cell resources for SR.
  unsigned nof_cell_sr_resources = 8;
  /// Number of PUCCH Format 1 cell resources for CSI.
  unsigned nof_cell_csi_resources = 8;

  /// \brief \c SR period in milliseconds.
  /// Among all values given in \c periodicityAndOffset, part of \c \SchedulingRequestResourceConfig, TS 38.331,
  /// these are the only ones supported. Values: {1, 2, 2.5, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320}.
  float sr_period_msec = 20.0F;

  /// PUCCH F0 resource parameter.
  /// Set true for PUCCH Format 0 intra-slot frequency hopping.
  bool f0_intraslot_freq_hopping = false;

  /// PUCCH F1 resource parameters.
  /// \defgroup pucch_f1_params
  /// \brief PUCCH F1 resource parameters.
  /// @{
  /// \brief Enable Orthogonal Cover Code for PUCCH Format 1.
  bool f1_enable_occ = false;
  /// \brief Number of different Initial Cyclic Shifts that can be used for PUCCH Format 1.
  /// Values: {1, 2, 3, 4, 6, 12}; 0 corresponds to "no cyclic shift".
  unsigned f1_nof_cyclic_shifts = 2;
  /// Set true for PUCCH Format 1 intra-slot frequency hopping.
  bool f1_intraslot_freq_hopping = false;
  /// @}

  /// PUCCH F2 resource parameters.
  /// \defgroup pucch_f2_params
  /// \brief PUCCH F2 resource parameters.
  /// @{
  /// Max number of PRBs for PUCCH Format 2. Values {1,...,16}.
  unsigned f2_max_nof_rbs = 1;
  /// \brief Min required payload capacity in bits that can be carried by PUCCH Format 2. Values {4,...,40}.
  /// If this is set, \ref f2_max_nof_rbs is ignored.
  std::optional<unsigned> f2_max_payload_bits;
  /// Max code rate for PUCCH Format 2.
  max_pucch_code_rate f2_max_code_rate = max_pucch_code_rate::dot_35;
  /// Set true for PUCCH Format 2 intra-slot frequency hopping. This field is ignored if f2_nof_symbols == 1.
  bool f2_intraslot_freq_hopping = false;
  /// @}

  /// PUCCH F3 resource parameters.
  /// \defgroup pucch_f3_params
  /// \brief PUCCH F3 resource parameters.
  /// @{
  /// Max number of PRBs for PUCCH Format 3. Values {1,...,16}.
  unsigned f3_max_nof_rbs = 1;
  /// \brief Min required payload capacity in bits that can be carried by PUCCH Format 3. Values {4,...,40}.
  /// If this is set, \ref f2_max_nof_rbs is ignored.
  std::optional<unsigned> f3_max_payload_bits;
  /// Max code rate for PUCCH Format 3.
  max_pucch_code_rate f3_max_code_rate = max_pucch_code_rate::dot_35;
  /// Set true for PUCCH Format 3 intra-slot frequency hopping.
  bool f3_intraslot_freq_hopping = false;
  /// Set true for PUCCH Format 3 additional DM-RS.
  bool f3_additional_dmrs = false;
  /// Set true to use pi/2-BPSK as the modulation for PUCCH Format 3.
  bool f3_pi2_bpsk = false;
  /// @}

  /// PUCCH F4 resource parameters.
  /// \defgroup pucch_f4_params
  /// \brief PUCCH F4 resource parameters.
  /// @{
  /// Max code rate for PUCCH Format 4.
  max_pucch_code_rate f4_max_code_rate = max_pucch_code_rate::dot_35;
  /// Set true for PUCCH Format 4 intra-slot frequency hopping.
  bool f4_intraslot_freq_hopping = false;
  /// Set true for PUCCH Format 4 additional DM-RS.
  bool f4_additional_dmrs = false;
  /// Set true to use pi/2-BPSK as the modulation for PUCCH Format 4.
  bool f4_pi2_bpsk = false;
  /// Enable Orthogonal Cover Code multiplexing for PUCCH Format 4.
  bool     f4_enable_occ = false;
  unsigned f4_occ_length = 2;
  /// @}

  /// Minimum k1 value (distance in slots between PDSCH and HARQ-ACK) that the gNB can use. Values: {1, ..., 7}.
  /// [Implementation-defined] As min_k1 is used for both common and dedicated PUCCH configuration, and in the UE
  /// fallback scheduler only allow max k1 = 7, we restrict min_k1 to 7.
  unsigned min_k1 = 4;

  /// Maximum number of consecutive undecoded PUCCH Format 2 for CSI before an RLF is reported.
  unsigned max_consecutive_kos = 100;

  /// Enable closed-loop PUCCH power control.
  bool enable_closed_loop_pw_control = false;
  /// Target PUSCH SINR to be achieved with close-loop power control, in dB. Only relevant if \c
  /// enable_closed_loop_pw_control is set to true and if the corresponding PUCCH format is used.
  /// SINR target are based on the requirements in terms of 1% ACK missed detection probability, as per TS 38.104,
  /// Tables 8.3.2.2-1 and 8.3.2.2-1 for Format 0, Table 8.3.4.1.2-1 and Table 8.3.4.1.2-2 for Format 2; while, for
  /// Format 3, they are based on the UCI block error probability not exceeding 1%, as per Table 8.3.5.2-1
  /// and 8.3.5.2-2.
  float pucch_f0_sinr_target_dB{10.0f};
  float pucch_f2_sinr_target_dB{6.0f};
  float pucch_f3_sinr_target_dB{1.0f};

  template <typename T>
  bool almost_equal(T a, T b) const
  {
    // This is only to detect if a given parameter differs from the default value.
    constexpr T tolerance = 0.01;
    return std::fabs(a - b) <= tolerance;
  }

  bool operator==(const du_high_unit_pucch_config& rhs) const
  {
    return p0_nominal == rhs.p0_nominal && pucch_resource_common == rhs.pucch_resource_common &&
           use_format_0 == rhs.use_format_0 && set1_format == rhs.set1_format &&
           nof_ue_pucch_res_harq_per_set == rhs.nof_ue_pucch_res_harq_per_set &&
           nof_cell_harq_pucch_sets == rhs.nof_cell_harq_pucch_sets &&
           nof_cell_sr_resources == rhs.nof_cell_sr_resources && nof_cell_csi_resources == rhs.nof_cell_csi_resources &&
           almost_equal<float>(sr_period_msec, rhs.sr_period_msec) &&
           f0_intraslot_freq_hopping == rhs.f0_intraslot_freq_hopping && f1_enable_occ == rhs.f1_enable_occ &&
           f1_nof_cyclic_shifts == rhs.f1_nof_cyclic_shifts &&
           f1_intraslot_freq_hopping == rhs.f1_intraslot_freq_hopping && f2_max_nof_rbs == rhs.f2_max_nof_rbs &&
           f2_max_payload_bits == rhs.f2_max_payload_bits && f2_max_code_rate == rhs.f2_max_code_rate &&
           f2_intraslot_freq_hopping == rhs.f2_intraslot_freq_hopping && f3_max_nof_rbs == rhs.f3_max_nof_rbs &&
           f3_max_payload_bits == rhs.f3_max_payload_bits && f3_max_code_rate == rhs.f3_max_code_rate &&
           f3_intraslot_freq_hopping == rhs.f3_intraslot_freq_hopping && f3_additional_dmrs == rhs.f3_additional_dmrs &&
           f3_pi2_bpsk == rhs.f3_pi2_bpsk && f4_max_code_rate == rhs.f4_max_code_rate &&
           f4_intraslot_freq_hopping == rhs.f4_intraslot_freq_hopping && f4_additional_dmrs == rhs.f4_additional_dmrs &&
           f4_pi2_bpsk == rhs.f4_pi2_bpsk && f4_enable_occ == rhs.f4_enable_occ && f4_occ_length == rhs.f4_occ_length &&
           min_k1 == rhs.min_k1 && max_consecutive_kos == rhs.max_consecutive_kos &&
           enable_closed_loop_pw_control == rhs.enable_closed_loop_pw_control &&
           almost_equal<float>(pucch_f0_sinr_target_dB, rhs.pucch_f0_sinr_target_dB) &&
           almost_equal<float>(pucch_f2_sinr_target_dB, rhs.pucch_f2_sinr_target_dB) &&
           almost_equal<float>(pucch_f3_sinr_target_dB, rhs.pucch_f3_sinr_target_dB);
  }
  bool operator!=(const du_high_unit_pucch_config& rhs) const { return !(*this == rhs); }
};

struct du_high_unit_srs_config {
  /// If set, enables periodic Sound Reference Signals (SRS) for the UEs within this cell. If not present, SRS are
  /// aperiodic. The given value is the SRS period in milliseconds.
  /// The available values are a subset of the values in \c SRS-PeriodicityAndOffset, \c SRS-Resource \c SRS-Config,
  /// TS 38.331, converted to millisecond.
  /// Values: {1, 2, 2.5, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560}.
  std::optional<float> srs_period_ms = std::nullopt;
  /// \brief Defines the maximum number of symbols dedicated to (all) the cell SRS resources in a slot.
  /// This is the space that the GNB reserves for all the cell SRS resources in the UL slots, not to be confused with
  /// the symbols per SRS resource configured in the UE dedicated configuration. Values: {1,...,6}.
  unsigned max_nof_symbols_per_slot = 2;
  /// Defines the number of symbols per SRS resource as per \c nrofSymbols, \c resourceMapping, \c SRS-Resource \c
  /// SRS-Config, TS 38.331. Values: {1, 2, 4}.
  unsigned nof_symbols = 1;
  /// \c Transmission comb number, \c transmissionComb, \c SRS-Resource \c SRS-Config, TS 38.331. Values: {2, 4}.
  unsigned tx_comb = 4;
  /// Defines the Cyclic Shift (CS) reuse factor for the SRS resources.
  /// \remark With 2 or 4 antenna ports, different cyclic shifts are used by the different antennas. This parameter
  /// defines how many UEs can be multiplexed in the same symbols and RBs by exploiting different cyclic shifts.
  /// Values: {no_cyclic_shift, two, four} for 2 UL antenna ports and tx_comb == 2.
  /// Values: {no_cyclic_shift, two, three, four, six} for 2 UL antenna ports and tx_comb == 4.
  /// Values: {no_cyclic_shift, two} for 4 UL antenna ports and tx_comb == 2.
  /// Values: {no_cyclic_shift, three} for 4 UL antenna ports and tx_comb == 4.
  /// Refer to Section 6.4.1.4.2, TS 38.211 for the definition of "Cyclic Shift".
  unsigned cyclic_shift_reuse_factor = 1;
  /// Defines the reuse of the SRS sequence ID for different UEs within the same cell.
  /// \remark The goal of the SRS sequence ID would be to reduce the inter-cell interference. However, if the cell is
  /// not in a dense multi-cell environment, we can reuse different sequence ID for different cell UEs.
  /// Values: {1, 2, 3, 5, 6, 10, 15, 30}.
  /// Refer to Section 6.4.1.4.2, TS 38.211 for the definition of "sequenceId".
  unsigned sequence_id_reuse_factor = 1;
  /// \c p0, TS 38.331. Value in dBm. Only even values allowed within {-202,...,24}.
  int p0 = -84;
};

/// Parameters that are used to initialize or build the \c PhysicalCellGroupConfig, TS 38.331.
struct du_high_unit_phy_cell_group_config {
  /// \brief \c p-NR-FR1, part of \c PhysicalCellGroupConfig, TS 38.331. Values: {-30,...,33}.
  /// The maximum total TX power to be used by the UE in this NR cell group across all serving cells in FR1.
  std::optional<int> p_nr_fr1;
};

/// TDD pattern configuration. See TS 38.331, \c TDD-UL-DL-Pattern.
struct tdd_ul_dl_pattern_unit_config {
  /// Periodicity of the DL-UL pattern in slots. Values {2,...,80}.
  unsigned dl_ul_period_slots = 10;
  /// Values: {0,...,maxNrofSlots=80}.
  unsigned nof_dl_slots = 6;
  /// Values: {0,...,maxNrofSymbols-1=13}.
  unsigned nof_dl_symbols = 8;
  /// Values: {0,...,maxNrofSlots=80}.
  unsigned nof_ul_slots = 3;
  /// Values: {0,...,maxNrofSymbols-1=13}.
  unsigned nof_ul_symbols = 0;
};

/// TDD configuration. See TS 38.331, \c TDD-UL-DL-ConfigCommon.
struct du_high_unit_tdd_ul_dl_config {
  tdd_ul_dl_pattern_unit_config                pattern1;
  std::optional<tdd_ul_dl_pattern_unit_config> pattern2;
};

/// Paging related configuration. See TS 38.331, PCCH-Config.
struct du_high_unit_paging_config {
  /// SearchSpace to use for Paging. Values {0, 1}.
  unsigned paging_search_space_id = 1;
  /// Default Paging cycle in nof. Radio Frames. Values {32, 64, 128, 256}.
  unsigned default_paging_cycle = 128;
  /// Number of paging frames per DRX cycle. Values {oneT, halfT, quarterT, oneEighthT, oneSixteethT}.
  pcch_config::nof_pf_per_drx_cycle nof_pf = pcch_config::nof_pf_per_drx_cycle::oneT;
  /// Paging frame offset. Values {0,...,(T/nof_pf_per_paging_cycle) - 1}.
  unsigned pf_offset = 0;
  /// Number of paging occasions per paging frame. Values {1, 2, 4}.
  unsigned nof_po_per_pf = 1;
};

/// PDCCH Common configuration.
struct pdcch_common_unit_config {
  /// CORESET#0 index as per tables in TS 38.213, clause 13.
  std::optional<unsigned> coreset0_index;
  /// Number of PDCCH candidates per aggregation level for SearchSpace#1. The aggregation level for the array element
  /// with index "x" is L=1U << x. The possible values for each element are {0, 1, 2, 3, 4, 5, 6, 8}.
  std::array<uint8_t, 5> ss1_n_candidates = {0, 0, 1, 0, 0};
  /// SearchSpace#0 index as per tables in TS 38.213, clause 13.
  unsigned ss0_index = 0;
  /// Maximum CORESET#0 duration in OFDM symbols to consider when deriving CORESET#0 index.
  std::optional<uint8_t> max_coreset0_duration;
};

/// PDCCH Dedicated configuration.
struct pdcch_dedicated_unit_config {
  /// Starting Common Resource Block (CRB) number for CORESET 1 relative to CRB 0.
  std::optional<unsigned> coreset1_rb_start;
  /// Length of CORESET 1 in number of CRBs.
  std::optional<unsigned> coreset1_l_crb;
  /// Duration of CORESET 1 in number of OFDM symbols.
  std::optional<unsigned> coreset1_duration;
  /// Number of PDCCH candidates per aggregation level for SearchSpace#2. The aggregation level for the array element
  /// with index "x" is L=1U << x. The possible values for each element are {0, 1, 2, 3, 4, 5, 6, 8}.
  /// NOTE: A value of {0, 0, 0, 0, 0} lets the gNB decide nof. candidates for SearchSpace#2.
  std::array<uint8_t, 5> ss2_n_candidates = {0, 0, 0, 0, 0};
  /// Flag specifying whether to use non-fallback or fallback DCI format in SearchSpace#2.
  bool dci_format_0_1_and_1_1 = true;
  /// SearchSpace type of SearchSpace#2.
  search_space_configuration::type_t ss2_type = search_space_configuration::type_t::ue_dedicated;
  /// Offset applied to the CQI for PDCCH aggregation level calculation.
  /// This allows fine-tuning of the aggregation level selection:
  ///  - A positive offset leads to a lower AL, resulting in a higher code rate and increased BLER.
  ///  - A positive offset leads to a higher AL, resulting in a lower code rate and reduced BLER.
  float al_cqi_offset = 0;
};

/// PDCCH application configuration.
struct du_high_unit_pdcch_config {
  /// PDCCH Common configuration applicable for entire cell.
  pdcch_common_unit_config common;
  /// PDCCH Dedicated configuration applicable for each UE.
  pdcch_dedicated_unit_config dedicated;
};

/// Configuration of SIBs and SI-message scheduling.
struct du_high_unit_sib_config {
  struct si_sched_info_config {
    /// List of SIB indexes (sib2 => value 2 in list, sib3 => value 3 in list, ...) included in this SI message. The
    /// list has at most 32 elements.
    std::vector<uint8_t> sib_mapping_info;
    /// Periodicity of the SI-message in radio frames. Values: {8, 16, 32, 64, 128, 256, 512}.
    unsigned si_period_rf = 32;
    /// SI window position of the associated SI-message. See TS 38.331, \c SchedulingInfo2-r17. Values: {1,...,256}.
    /// \remark This field is only applicable for release 17 \c SI-SchedulingInfo2.
    std::optional<unsigned> si_window_position;
  };

  /// \brief Earthquake and Tsunami Warning System (ETWS) message parameters.
  ///
  /// ETWS messages are broadcasted over SIB 6 and SIB 7. SIB 6 carries the ETWS primary notification, while SIB-7
  /// carries the secondary notification.
  struct etws_config {
    /// \brief ETWS message ID (see \ref sib6_info::message_id). Values: {0, ..., 0xffff}
    /// \remark See TS23.041 Section 9.4.1.2.2 for a list of meaningful values.
    unsigned message_id = 0x1104;
    /// \brief ETWS message serial number (see \ref sib6_info::serial_number). Values: {0, ..., 0xffff}
    /// \remark See TS23.041 Section 9.4.1.2.1 for a list of meaningful values.
    unsigned serial_num = 0x3000;
    /// \brief ETWS warning type (see \ref sib6_info::warning_type). Values: {0, ..., 0xffff}
    /// \remark See TS23.041 Section 9.3.24 for a list of meaningful values.
    unsigned warning_type = 0x0980;
    /// \brief CBS Coding scheme used for the warning message Values: {0, ..., 0xff}.
    ///
    /// Supported coding schemes:
    ///   - 0x00..0x0f: Languages using GSM-7 default alphabet.
    ///   - 0x40..0x4f: General data coding indication (uncompressed text, no message class meaning).
    ///     Set bits 3..2 to 0b00 for GSM-7 or 0b10 for UCS-2. Other character sets are not supported.
    ///   - 0x50..0x5f: General data coding indication (uncompressed text, message class meaning).
    ///     Set bits 3..2 to 0b00 for GSM-7 or 0b10 for UCS-2. Other character sets are not supported.
    ///   - 0xf0..0xff: Data coding / message handling.
    ///     Bit 2 must be set to 0 (GSM-7 encoding).
    ///
    /// \remark See TS23.038 Section 5 for a list of meaningful values.
    unsigned data_coding_scheme = 0x00;
    /// \brief ETWS warning message.
    ///
    /// Character support depends on the chosen coding scheme (see \ref data_coding_scheme).
    ///
    /// \remark Required if SIB-7 is scheduled in \ref si_sched_info, otherwise leave unset.
    std::optional<std::string> warning_message;
  };

  /// \brief Commercial Mobile Alert Service (CMAS) message parameters.
  ///
  /// CMAS messages are broadcasted over SIB 8.
  struct cmas_config {
    /// \brief CMAS message ID (see \ref sib8_info::message_id). Values: {0, ..., 0xffff}
    /// \remark See TS23.041 Section 9.4.1.2.2 for a list of meaningful values.
    unsigned message_id = 0x1112;
    /// \brief CMAS message serial number (see \ref sib8_info::serial_number). Values: {0, ..., 0xffff}
    /// \remark See TS23.041 Section 9.4.1.2.1 for a list of meaningful values.
    unsigned serial_num = 0x3003;
    /// \brief CBS Coding scheme used for the warning message Values: {0, ..., 0xff}.
    ///
    /// Supported coding schemes:
    ///   - 0x00..0x0f: Languages using GSM-7 default alphabet.
    ///   - 0x40..0x4f: General data coding indication (uncompressed text, no message class meaning).
    ///     Set bits 3..2 to 0b00 for GSM-7 or 0b10 for UCS-2. Other character sets are not supported.
    ///   - 0x50..0x5f: General data coding indication (uncompressed text, message class meaning).
    ///     Set bits 3..2 to 0b00 for GSM-7 or 0b10 for UCS-2. Other character sets are not supported.
    ///   - 0xf0..0xff: Data coding / message handling.
    ///     Bit 2 must be set to 0 (GSM-7 encoding).
    ///
    /// \remark See TS23.038 Section 5 for a list of meaningful values.
    unsigned data_coding_scheme = 0x00;
    /// \brief CMAS warning message.
    /// \remark Character support depends on the chosen coding scheme (see \ref data_coding_scheme).
    std::string warning_message;
  };

  struct sib_ue_timers_and_constants {
    /// t300
    /// Values (in ms): {100, 200, 300, 400, 600, 1000, 1500, 2000}
    unsigned t300 = 1000;
    /// t301
    /// Values (in ms): {100, 200, 300, 400, 600, 1000, 1500, 2000}
    unsigned t301 = 1000;
    /// t310
    /// Values (in ms): {0, 50, 100, 200, 500, 1000, 2000}
    unsigned t310 = 1000;
    /// n310
    /// Values: {1, 2, 3, 4, 6, 8, 10, 20}
    unsigned n310 = 1;
    /// t311
    /// Values (in ms): {1000, 3000, 5000, 10000, 15000, 20000, 30000}
    unsigned t311 = 3000;
    /// n311
    /// Values: {1, 2, 3, 4, 5, 6, 8, 10}
    unsigned n311 = 1;
    /// t319
    /// Values (in ms): {100, 200, 300, 400, 600, 1000, 1500, 2000}
    unsigned t319 = 1000;
  };

  /// The length of the SI scheduling window, in slots. It is always shorter or equal to the period of the SI message.
  /// Values: {5, 10, 20, 40, 80, 160, 320, 640, 1280}.
  unsigned si_window_len_slots = 160;
  /// List of SI-messages and associated scheduling information.
  std::vector<si_sched_info_config> si_sched_info;
  /// UE timers and constants parameters
  sib_ue_timers_and_constants ue_timers_and_constants;
  /// Parameters of the SIB19.
  sib19_info sib19;
  /// ETWS configuration parameters.
  std::optional<etws_config> etws_cfg;
  /// CMAS configuration parameters.
  std::optional<cmas_config> cmas_cfg;
};

struct du_high_unit_csi_config {
  /// \brief Enable CSI-RS and CSI reporting in the cell.
  bool csi_rs_enabled = true;
  /// \brief \c CSI-RS period in milliseconds. Limited by TS38.214, clause 5.1.6.1.1. Values: {10, 20, 40, 80}.
  unsigned csi_rs_period_msec = 20;
  /// \brief Slot offset for measurement CSI-RS resources. If not set, it is automatically derived to avoid collisions
  /// with SSB and SIB1.
  std::optional<unsigned> meas_csi_slot_offset;
  /// \brief Slot offset of the first CSI-RS resource used for tracking. If not set, it is automatically derived to
  /// avoid collisions with SSB and SIB1.
  std::optional<unsigned> tracking_csi_slot_offset;
  /// \brief Slot offset for the zp-CSI-RS resources. If not set, it is automatically derived to avoid collisions with
  /// SSB and SIB1.
  std::optional<unsigned> zp_csi_slot_offset;
  /// \brief \c powerControlOffset, part of \c NZP-CSI-RS-Resource, as per TS 38.331.
  /// Power offset of PDSCH RE to NZP CSI-RS RE. Value in dB {-8,...,15}.
  int pwr_ctrl_offset = 0;
};

/// MAC Buffer Status Report application configuration.
struct mac_bsr_unit_config {
  /// Periodic Buffer Status Report Timer value in nof. subframes. Values {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160,
  /// 320, 640, 1280, 2560, 0}.
  /// \remark Value 0 equates to periodicBSR-Timer value of infinity.
  unsigned periodic_bsr_timer = 10;
  /// Retransmission Buffer Status Report Timer value in nof. subframes. Values {10, 20, 40, 80, 160, 320, 640, 1280,
  /// 2560, 5120, 10240}.
  unsigned retx_bsr_timer = 80;
  /// Logical Channel SR delay timer in nof. subframes. Values {20, 40, 64, 128, 512, 1024, 2560}.
  std::optional<unsigned> lc_sr_delay_timer;
};

/// MAC Power Headroom Reporting configuration.
struct mac_phr_unit_config {
  /// \brief \c phr-ProhibitTimer, value in number of subframes for PHR reporting.
  /// Values: {0, 10, 20, 50, 100, 200, 500, 1000}.
  unsigned phr_prohib_timer = 10;
};

/// MAC Scheduler Request configuration.
struct mac_sr_unit_config {
  /// \brief \c sr-ProhibitTimer, or timer for SR transmission on PUCCH.
  /// Values are in ms. Values: {1, 2, 4, 8, 16, 32, 64, 128}. When the field is absent, the UE applies the value 0.
  std::optional<unsigned> sr_prohibit_timer;
  /// \brief \c sr-TransMax possible values, or maximum number of SR transmissions.
  /// Values: {4, 8, 16, 32, 64}.
  unsigned sr_trans_max = 64;
};

// MAC Cell group application configuration.
struct du_high_unit_mac_cell_group_config {
  /// Buffer Status Report configuration.
  mac_bsr_unit_config bsr_cfg;
  /// Power Headroom Reporting configuration.
  mac_phr_unit_config phr_cfg;
  /// Scheduler Request configuration
  mac_sr_unit_config sr_cfg;
};

/// PRACH application configuration.
struct du_high_unit_prach_config {
  /// PRACH configuration index. If not specified, it is automatically derived to fit in an UL slot.
  std::optional<unsigned> prach_config_index;
  /// PRACH root sequence index.
  unsigned prach_root_sequence_index = 1;
  /// Zero correlation zone
  unsigned zero_correlation_zone = 0;
  unsigned fixed_msg3_mcs        = 0;
  unsigned max_msg3_harq_retx    = 4;
  /// \c preambleReceivedTargetPower, target power at the network rx side, in dBm. Only values multiple of 2 are
  /// valid.
  int preamble_rx_target_pw = -100;
  /// Total number of PRACH preambles used for contention based and contention free random access in the RACH resources
  /// in the RACH-ConfigCommon.
  unsigned total_nof_ra_preambles = 64;
  /// \brief Whether to enable contention-free random access (CFRA) or not. If enabled, the number of RA preambles
  /// used for CBRA (see \c nof_cb_preambles_per_ssb) must be less than \c total_nof_ra_preambles.
  bool cfra_enabled = false;
  /// Offset of lowest PRACH transmission occasion in frequency domain respective to PRB 0. To minimize interference
  /// with the PUCCH, the user should leave some guardband between the PUCCH CRBs and the PRACH PRBs.
  /// Possible values: {0,...,MAX_NOF_PRB - 1}.
  std::optional<unsigned> prach_frequency_start;
  /// Max number of RA preamble transmissions performed before declaring a failure. Values {3, 4, 5, 6, 7, 8, 10, 20,
  /// 50, 100, 200}.
  uint8_t preamble_trans_max = 7;
  /// Power ramping steps for PRACH. Values {0, 2, 4, 6}.
  uint8_t power_ramping_step_db = 4;
  /// Ports list for PRACH reception.
  std::vector<uint8_t> ports = {0};
  /// Indicates the number of SSBs per RACH occasion (L1 parameter 'SSB-per-rach-occasion'). See TS 38.331, \c
  /// ssb-perRACH-OccasionAndCB-PreamblesPerSSB.
  ssb_per_rach_occasions nof_ssb_per_ro = ssb_per_rach_occasions::one;
  /// Indicates the number of Contention Based preambles per SSB (L1 parameter 'CB-preambles-per-SSB'). See TS 38.331,
  /// \c ssb-perRACH-OccasionAndCB-PreamblesPerSSB.
  /// \remark Values of \c cb_preambles_per_ssb depends on value of \c ssb_per_ro.
  uint8_t nof_cb_preambles_per_ssb = 64;
  /// RA-Response (MSG2) window length in number of slots. Values: {1, 2, 4, 8, 10, 20, 40, 80}.  If not specified, it
  /// is automatically derived to be equal to 10ms.
  std::optional<unsigned> ra_resp_window;
};

/// Slice scheduling configuration for a cell.
struct du_high_unit_cell_slice_sched_config {
  /// Sets the minimum percentage of PRBs to be allocated to this group.
  unsigned min_prb_policy_ratio = 0;
  /// Sets the maximum percentage of PRBs to be allocated to this group.
  unsigned max_prb_policy_ratio = 100;
  /// Sets the slice priority. Values: {0,...,254}. 255 is reserved for the SRBs.
  unsigned priority = 0;
  /// Policy scheduler parameters for the slice. Default: Time-domain round robin.
  std::optional<policy_scheduler_expert_config> slice_policy_sched_cfg;
};

/// Radio Link Monitoring Config for a cell.
struct du_high_unit_rlm_config {
  /// Resource type used for Radio Resource Monitoring.
  /// "default_type" sets the default resources (the same as used for TCI states for PDCCH reception).
  /// "ssb" sets the only SSB resource of the cell.
  /// "csi_rs" sets the CSI-RS resources used for tracking, if enabled.
  /// "ssb_and_csi_rs" sets both SSB and CSI-RS resources used for tracking.
  rlm_resource_type resource_type = rlm_resource_type::default_type;
};

/// Slice configuration for a cell.
struct du_high_unit_cell_slice_config {
  /// Slice/Service Type.
  uint8_t sst;
  /// Slice Differentiator.
  uint32_t sd = 0xffffffU;
  /// Slice scheduling configuration.
  du_high_unit_cell_slice_sched_config sched_cfg;
};

/// Base cell configuration.
struct du_high_unit_base_cell_config {
  /// Physical cell identifier.
  pci_t pci = 1;
  /// Sector Id (4-14 bits) that gets concatenated with gNB-Id to form the NR Cell Identity (NCI).
  std::optional<unsigned> sector_id;
  /// DL ARFCN of "F_REF", which is the RF reference frequency, as per TS 38.104, Section 5.4.2.1.
  unsigned dl_f_ref_arfcn = 536020;
  /// Common subcarrier spacing for the entire resource grid. It must be supported by the band SS raster.
  subcarrier_spacing common_scs = subcarrier_spacing::kHz15;
  /// NR band.
  std::optional<nr_band> band;
  /// Channel bandwidth in MHz.
  bs_channel_bandwidth channel_bw_mhz = bs_channel_bandwidth::MHz20;
  /// Number of antennas in downlink.
  unsigned nof_antennas_dl = 1;
  /// Number of antennas in uplink.
  unsigned nof_antennas_ul = 1;
  /// Human readable full PLMN (without possible filler digit).
  std::string plmn = "00101";
  /// List of human readable PLMNs (without possible filler digit) that are additionally supported by the cell.
  std::vector<std::string> additional_plmns;
  /// TAC.
  tac_t tac = 7;
  /// Whether the DU adds this cell to the list of served cells while communicating with the CU-CP or it waits for a
  /// command from the SMO to activate it.
  bool enabled = true;
  /// \c q-RxLevMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dBm.
  int q_rx_lev_min = -70;
  /// \c q-QualMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dB.
  int q_qual_min = -20;
  /// SSB parameters.
  du_high_unit_ssb_config ssb_cfg;
  /// SIB parameters.
  du_high_unit_sib_config sib_cfg;
  /// UL common configuration parameters.
  du_high_unit_ul_common_config ul_common_cfg;
  /// PDCCH configuration.
  du_high_unit_pdcch_config pdcch_cfg;
  /// PDSCH configuration.
  du_high_unit_pdsch_config pdsch_cfg;
  /// PRACH configuration.
  du_high_unit_prach_config prach_cfg;
  /// PUSCH configuration.
  du_high_unit_pusch_config pusch_cfg;
  /// PUCCH configuration.
  du_high_unit_pucch_config pucch_cfg;
  /// SRS configuration.
  du_high_unit_srs_config srs_cfg;
  /// Physical Cell Group parameters.
  du_high_unit_phy_cell_group_config pcg_cfg;
  /// MAC Cell Gropup parameters.
  du_high_unit_mac_cell_group_config mcg_cfg;
  /// TDD slot configuration.
  std::optional<du_high_unit_tdd_ul_dl_config> tdd_ul_dl_cfg;
  /// Paging configuration.
  du_high_unit_paging_config paging_cfg;
  /// CSI configuration.
  du_high_unit_csi_config csi_cfg;
  /// Scheduler expert configuration.
  du_high_unit_scheduler_expert_config sched_expert_cfg;
  /// DRX configuration used when cell corresponds to PCell.
  du_high_unit_drx_config drx_cfg;
  /// Network slice configuration.
  std::vector<du_high_unit_cell_slice_config> slice_cfg;
  /// NTN configuration.
  std::optional<ntn_config> ntn_cfg;
  /// Radio Link Monitoring configuration.
  du_high_unit_rlm_config rlm_cfg;
};

struct du_high_unit_test_mode_ue_config {
  /// C-RNTI to assign to the test UE.
  rnti_t rnti = rnti_t::INVALID_RNTI;
  /// Number of test UE(s) to create.
  uint16_t nof_ues = 1;
  /// \brief Delay, in slots, before the MAC test mode auto-generates the UCI/CRC indication to pass to the scheduler.
  /// This feature should be avoided if the OFH/UL PHY are operational, otherwise the auto-generated indications
  /// may interfere with the UL PHY HARQ handling.
  std::optional<unsigned> auto_ack_indication_delay;
  /// Whether PDSCH grants are automatically assigned to the test UE.
  bool pdsch_active = true;
  /// Whether PUSCH grants are automatically assigned to the test UE.
  bool pusch_active = true;
  /// Channel Quality Indicator to use for the test UE.
  unsigned cqi = 15;
  /// Rank Indicator to use for the test UE. This value has to be lower than the number of ports.
  unsigned ri = 1;
  /// Precoding Matrix Indicators to use for the test UE.
  unsigned pmi = 0;
  /// Precoding codebook indexes to be used in case of more than 2 antenna ports.
  unsigned i_1_1 = 0;
  unsigned i_1_3 = 0;
  unsigned i_2   = 0;
};

/// gNB app Test Mode configuration.
struct du_high_unit_test_mode_config {
  /// Creates a UE with the given the given params for testing purposes.
  du_high_unit_test_mode_ue_config test_ue;
};

/// Cell configuration
struct du_high_unit_cell_config {
  /// Cell configuration.
  du_high_unit_base_cell_config cell;
};

/// Configuration to enable/disable metrics per layer.
struct du_high_unit_metrics_layer_config {
  bool enable_scheduler = true;
  bool enable_rlc       = false;
  bool enable_mac       = false;

  /// Returns true if one or more layers are enabled, otherwise false.
  bool are_metrics_enabled() const { return enable_scheduler || enable_rlc || enable_mac; }
};

/// Metrics report configuration.
struct du_high_unit_metrics_config {
  /// Scheduler report period in milliseconds.
  unsigned                          du_report_period         = 1000;
  bool                              autostart_stdout_metrics = false;
  app_helpers::metrics_config       common_metrics_cfg;
  du_high_unit_metrics_layer_config layers_cfg;
};

struct du_high_unit_pcap_config {
  struct {
    std::string filename = "/tmp/du_f1ap.pcap";
    bool        enabled  = false;
  } f1ap;
  struct {
    std::string filename = "/tmp/du_f1u.pcap";
    bool        enabled  = false;
  } f1u;
  struct {
    std::string filename = "/tmp/du_rlc.pcap";
    std::string rb_type  = "all";
    bool        enabled  = false;
  } rlc;
  struct {
    std::string filename = "/tmp/du_mac.pcap";
    std::string type     = "udp";
    bool        enabled  = false;
  } mac;

  /// When using the gNB app, there is no point in instantiating
  /// F1 pcaps twice. This function force disables them.
  /// TODO: revisit
  void disable_f1_pcaps()
  {
    f1u.enabled  = false;
    f1ap.enabled = false;
  }
};

/// Configuration of the task executors queues.
struct du_high_unit_execution_queues_config {
  uint32_t ue_data_executor_queue_size = 8192;
};

/// Expert configuration of the DU high.
struct du_high_unit_expert_execution_config {
  /// \brief Task executor configuration for the DU.
  du_high_unit_execution_queues_config du_queue_cfg;
};

/// RLC UM TX configuration
struct du_high_unit_rlc_tx_am_config {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  int32_t  t_poll_retx;     ///< Poll retx timeout (ms)
  uint32_t max_retx_thresh; ///< Max retx threshold
  int32_t  poll_pdu;        ///< Insert poll bit after this many PDUs
  int32_t  poll_byte;       ///< Insert poll bit after this much data (bytes)
  uint32_t max_window = 0;  ///< Custom parameter to limit the maximum window size for memory reasons. 0 means no limit.
  uint32_t queue_size = 4096;              ///< RLC SDU queue size
  uint32_t queue_size_bytes = 4096 * 1507; ///< RLC SDU queue size in bytes
};

/// RLC UM RX configuration
struct du_high_unit_rlc_rx_am_config {
  uint16_t sn_field_length;   ///< Number of bits used for sequence number
  int32_t  t_reassembly;      ///< Timer used by rx to detect PDU loss (ms)
  int32_t  t_status_prohibit; ///< Timer used by rx to prohibit tx of status PDU (ms)

  // Implementation-specific parameters that are not specified by 3GPP
  /// Maximum number of visited SNs in the RX window when building a status report. 0 means no limit.
  uint32_t max_sn_per_status = 0;
};

/// RLC AM configuration
struct du_high_unit_rlc_am_config {
  du_high_unit_rlc_tx_am_config tx;
  du_high_unit_rlc_rx_am_config rx;
};

/// QoS configuration
struct du_high_unit_srb_config {
  unsigned                   srb_id;
  du_high_unit_rlc_am_config rlc;
};

/// F1-U configuration at DU side
struct du_high_unit_f1u_du_config {
  int32_t  t_notify;          ///< Maximum backoff time for transmit/delivery notifications from DU to CU_UP (ms)
  uint32_t ul_buffer_timeout; ///< Timeout for UL buffering that waits for the handover to fully finish.
  uint32_t ul_buffer_size;    ///< UL buffer size to be used during handover.
};

/// RLC UM TX configuration
struct du_high_unit_rlc_tx_um_config {
  uint16_t sn_field_length;  ///< Number of bits used for sequence number
  uint32_t queue_size;       ///< RLC SDU queue size in pdus
  uint32_t queue_size_bytes; ///< RLC SDU queue size in bytes
};

/// RLC UM RX configuration
struct du_high_unit_rlc_rx_um_config {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  int32_t  t_reassembly;    ///< Timer used by rx to detect PDU loss (ms)
};

/// RLC UM configuration
struct du_high_unit_rlc_um_config {
  du_high_unit_rlc_tx_um_config tx;
  du_high_unit_rlc_rx_um_config rx;
};

/// RLC configuration
struct du_high_unit_rlc_config {
  std::string                mode = "am";
  du_high_unit_rlc_um_config um;
  du_high_unit_rlc_am_config am;
};

/// QoS configuration
struct du_high_unit_qos_config {
  five_qi_t                  five_qi = uint_to_five_qi(9);
  du_high_unit_rlc_config    rlc;
  du_high_unit_f1u_du_config f1u_du;
};

/// DU high configuration.
struct du_high_unit_config {
  bool warn_on_drop = false;
  /// gNodeB identifier.
  gnb_id_t gnb_id = {411, 22};
  /// DU identifier.
  gnb_du_id_t gnb_du_id = gnb_du_id_t::min;
  /// PCAPs.
  du_high_unit_pcap_config pcaps;
  /// Metrics.
  du_high_unit_metrics_config metrics;
  /// Loggers.
  du_high_unit_logger_config loggers;
  /// Tracer.
  du_high_unit_tracer_config tracer;
  /// Configuration for testing purposes.
  du_high_unit_test_mode_config test_mode_cfg = {};
  /// \brief Cell configuration.
  ///
  /// \note Add one cell by default.
  std::vector<du_high_unit_cell_config> cells_cfg = {{}};
  /// QoS configuration.
  std::vector<du_high_unit_qos_config> qos_cfg;
  /// DU high expert execution settings.
  du_high_unit_expert_execution_config expert_execution_cfg;
  /// SRB configuration.
  std::map<srb_id_t, du_high_unit_srb_config> srb_cfg;

  /// Returns true if testmode is enabled, false otherwise.
  bool is_testmode_enabled() const { return test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI; }
};

/// DU high configuration.
struct du_high_parsed_config {
  du_high_unit_config config;

  /// \brief Base cell application configuration.
  ///
  /// \note When a cell is added, it will use the values of this base cell as default values for its base cell
  /// configuration. This parameter usage is restricted for filling cell information in the \remark cell_cfg
  /// variable.
  du_high_unit_base_cell_config common_cell_cfg;
};

} // namespace srsran
