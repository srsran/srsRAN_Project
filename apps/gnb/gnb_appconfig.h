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

#include "gnb_os_sched_affinity_manager.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/direct_current_offset.h"
#include "srsran/ran/five_qi.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/ntn.h"
#include "srsran/ran/pcch/pcch_configuration.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/ran/sib/system_info_config.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/unique_thread.h"
#include "srsran/support/units.h"
#include <map>
#include <string>
#include <thread>
#include <vector>

namespace srsran {

/// PRACH application configuration.
struct prach_appconfig {
  /// PRACH configuration index. If not specified, it is automatically derived to fit in an UL slot.
  optional<unsigned> prach_config_index;
  /// PRACH root sequence index.
  unsigned prach_root_sequence_index = 1;
  /// Zero correlation zone
  unsigned zero_correlation_zone = 0;
  unsigned fixed_msg3_mcs        = 0;
  unsigned max_msg3_harq_retx    = 4;
  /// \c preambleReceivedTargetPower, target power at the network rx side, in dBm. Only values multiple of 2 are valid.
  int preamble_rx_target_pw = -100;
  /// Total number of PRACH preambles used for contention based and contention free 4-step or 2-step random access.
  optional<unsigned> total_nof_ra_preambles;
  /// Offset of lowest PRACH transmission occasion in frequency domain respective to PRB 0. To minimize interference
  /// with the PUCCH, the user should leave some guardband between the PUCCH CRBs and the PRACH PRBs.
  /// Possible values: {0,...,MAX_NOF_PRB - 1}.
  optional<unsigned> prach_frequency_start;
  /// Max number of RA preamble transmissions performed before declaring a failure. Values {3, 4, 5, 6, 7, 8, 10, 20,
  /// 50, 100, 200}.
  uint8_t preamble_trans_max = 7;
  /// Power ramping steps for PRACH. Values {0, 2, 4, 6}.
  uint8_t power_ramping_step_db = 4;
  /// Ports list for PRACH reception.
  std::vector<uint8_t> ports = {0};
};

/// TDD pattern configuration. See TS 38.331, \c TDD-UL-DL-Pattern.
struct tdd_ul_dl_pattern_appconfig {
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
struct tdd_ul_dl_appconfig {
  tdd_ul_dl_pattern_appconfig           pattern1;
  optional<tdd_ul_dl_pattern_appconfig> pattern2;
};

/// Paging related configuration. See TS 38.331, PCCH-Config.
struct paging_appconfig {
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
struct pdcch_common_appconfig {
  /// CORESET#0 index as per tables in TS 38.213, clause 13.
  optional<unsigned> coreset0_index;
  /// Number of PDCCH candidates per aggregation level for SearchSpace#1. The aggregation level for the array element
  /// with index "x" is L=1U << x. The possible values for each element are {0, 1, 2, 3, 4, 5, 6, 8}.
  std::array<uint8_t, 5> ss1_n_candidates = {0, 0, 1, 0, 0};
  /// SearchSpace#0 index as per tables in TS 38.213, clause 13.
  unsigned ss0_index = 0;
  /// Maximum CORESET#0 duration in OFDM symbols to consider when deriving CORESET#0 index.
  optional<uint8_t> max_coreset0_duration;
};

/// PDCCH Dedicated configuration.
struct pdcch_dedicated_appconfig {
  /// Starting Common Resource Block (CRB) number for CORESET 1 relative to CRB 0.
  optional<unsigned> coreset1_rb_start;
  /// Length of CORESET 1 in number of CRBs.
  optional<unsigned> coreset1_l_crb;
  /// Duration of CORESET 1 in number of OFDM symbols.
  optional<unsigned> coreset1_duration;
  /// Number of PDCCH candidates per aggregation level for SearchSpace#2. The aggregation level for the array element
  /// with index "x" is L=1U << x. The possible values for each element are {0, 1, 2, 3, 4, 5, 6, 8}.
  /// NOTE: A value of {0, 0, 0, 0, 0} lets the gNB decide nof. candidates for SearchSpace#2.
  std::array<uint8_t, 5> ss2_n_candidates = {0, 0, 0, 0, 0};
  /// Flag specifying whether to use non-fallback or fallback DCI format in SearchSpace#2.
  bool dci_format_0_1_and_1_1 = true;
  /// SearchSpace type of SearchSpace#2.
  search_space_configuration::type_t ss2_type = search_space_configuration::type_t::ue_dedicated;
};

/// PDCCH application configuration.
struct pdcch_appconfig {
  /// PDCCH Common configuration applicable for entire cell.
  pdcch_common_appconfig common;
  /// PDCCH Dedicated configuration applicable for each UE.
  pdcch_dedicated_appconfig dedicated;
};

/// PDSCH application configuration.
struct pdsch_appconfig {
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
  /// Number of UE DL HARQ processes.
  unsigned nof_harqs = 16;
  /// Maximum number of times an HARQ process can be retransmitted, before it gets discarded.
  unsigned max_nof_harq_retxs = 4;
  /// Maximum number of consecutive DL KOs before an RLF is reported.
  unsigned max_consecutive_kos = 100;
  /// Redundancy version sequence to use. Each element can have one of the following values: {0, 1, 2, 3}.
  std::vector<unsigned> rv_sequence = {0, 2, 3, 1};
  /// MCS table to use for PDSCH
  pdsch_mcs_table mcs_table = pdsch_mcs_table::qam64;
  /// Minimum number of RBs for Resource Allocation of UE PDSCHs.
  unsigned min_rb_size = 1;
  /// Maximum number of RBs for Resource Allocation of UE PDSCHs.
  unsigned max_rb_size = MAX_NOF_PRBS;
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
  /// Link Adaptation (LA) threshold for drop in CQI of the first HARQ transmission above which HARQ retransmissions are
  /// cancelled.
  uint8_t harq_la_cqi_drop_threshold{3};
  /// Link Adaptation (LA) threshold for drop in nof. layers of the first HARQ transmission above which HARQ
  /// retransmission is cancelled.
  uint8_t harq_la_ri_drop_threshold{1};
};

/// PUSCH application configuration.
struct pusch_appconfig {
  /// \brief Minimum modulation and coding scheme index for C-RNTI PUSCH allocations. Note that setting a high minimum
  /// MCS may lead to a high BLER if the SINR is low.
  unsigned min_ue_mcs = 0;
  /// Maximum modulation and coding scheme index for C-RNTI PUSCH allocations. To set a fixed MCS, set \c min_ue_mcs
  /// equal to the \c max_ue_mcs.
  unsigned max_ue_mcs = 28;
  /// Maximum number of consecutive UL KOs before an RLF is reported.
  unsigned max_consecutive_kos = 100;
  /// Redundancy version sequence to use. Each element can have one of the following values: {0, 1, 2, 3}.
  std::vector<unsigned> rv_sequence = {0};
  /// MCS table to use for PUSCH
  pusch_mcs_table mcs_table = pusch_mcs_table::qam64;
  /// \c msg3-DeltaPreamble, TS 38.331. Values: {-1,...,6}.
  int msg3_delta_preamble = 6;
  /// \c p0-NominalWithGrant, TS 38.331. Value in dBm. Only even values allowed within {-202,...,24}.
  int p0_nominal_with_grant = -76;

  /// \c betaOffsetACK-Index1, \c BetaOffsets, TS 38.331. Values: {0,...,31}.
  unsigned b_offset_ack_idx_1 = 9;
  /// \c betaOffsetACK-Index2, \c BetaOffsets, TS 38.331. Values: {0,...,31}.
  unsigned b_offset_ack_idx_2 = 9;
  /// \c betaOffsetACK-Index3, \c BetaOffsets, TS 38.331. Values: {0,...,31}.
  unsigned b_offset_ack_idx_3 = 9;
  /// \c betaOffsetCSI-Part1-Index1, \c BetaOffsets, TS 38.331. Values: {0,...,31}.
  unsigned b_offset_csi_p1_idx_1 = 9;
  /// \c betaOffsetCSI-Part1-Index2, \c BetaOffsets, TS 38.331. Values: {0,...,31}.
  unsigned b_offset_csi_p1_idx_2 = 9;
  /// \c betaOffsetCSI-Part2-Index1, \c BetaOffsets, TS 38.331. Values: {0,...,31}.
  unsigned b_offset_csi_p2_idx_1 = 9;
  /// \c betaOffsetCSI-Part2-Index2, \c BetaOffsets, TS 38.331. Values: {0,...,31}.
  unsigned b_offset_csi_p2_idx_2 = 9;

  /// \brief Power level corresponding to MSG-3 TPC command in dB, as per Table 8.2-2, TS 38.213.
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
};

struct pucch_appconfig {
  /// \c PUCCH-ConfigCommon parameters.
  /// \c p0-nominal, TS 38.331. Value in dBm. Only even values allowed within {-202,...,24}.
  int p0_nominal = -90;

  /// \c PUCCH-Config parameters.
  /// Number of PUCCH Format 1 resources per UE for HARQ-ACK reporting. Values {1,...,8}.
  unsigned nof_ue_pucch_f1_res_harq = 8;
  /// Number of PUCCH Format 2 resources per UE for HARQ-ACK reporting. Values {1,...,8}.
  unsigned nof_ue_pucch_f2_res_harq = 6;
  /// \brief Number of separate PUCCH resource sets for HARQ-ACK reporting that are available in a cell.
  /// \remark UEs will be distributed possibly over different HARQ-ACK PUCCH sets; the more sets, the fewer UEs will
  /// have to share the same set, which reduces the chances that UEs won't be allocated PUCCH due to lack of resources.
  /// However, the usage of PUCCH-dedicated REs will be proportional to the number of sets.
  unsigned nof_cell_harq_pucch_sets = 2;
  /// Number of PUCCH Format 1 cell resources for SR.
  unsigned nof_cell_sr_resources = 8;
  /// Number of PUCCH Format 1 cell resources for CSI.
  unsigned nof_cell_csi_resources = 8;

  /// \brief \c SR period in milliseconds.
  /// Among all values given in \c periodicityAndOffset, part of \c \SchedulingRequestResourceConfig, TS 38.331,
  /// these are the only ones supported. Values: {1, 2, 4, 8, 10, 16, 20, 40, 80, 160, 320}.
  unsigned sr_period_msec = 20;

  /// PUCCH F1 resource parameters.
  /// Number of symbols for PUCCH Format 1. Values {4, 14}.
  unsigned f1_nof_symbols = 14;
  bool     f1_enable_occ  = true;
  /// \brief Number of different Initial Cyclic Shifts that can be used for PUCCH Format 1.
  /// Values: {1, 2, 3, 4, 6, 12}; 0 corresponds to "no cyclic shift".
  unsigned nof_cyclic_shift = 2;
  /// Set true for PUCCH Format 1 intra-slot frequency hopping.
  bool f1_intraslot_freq_hopping = false;

  /// PUCCH F2 resource parameters.
  /// Number of symbols for PUCCH Format 2. Values {1, 2}.
  unsigned f2_nof_symbols = 2;
  /// Max number of PRBs for PUCCH Format 2. Values {1,...,16}.
  unsigned f2_max_nof_rbs = 1;
  /// \brief Maximum payload in bits that can be carried by PUCCH Format 2. Values {-1,...,11}.
  /// Value -1 to unset. If this is set, \ref f2_max_nof_rbs is ignored.
  optional<unsigned> max_payload_bits;
  /// Set true for PUCCH Format 2 intra-slot frequency hopping. This field is ignored if f2_nof_symbols == 1.
  bool f2_intraslot_freq_hopping = false;
  /// Max code rate.
  max_pucch_code_rate max_code_rate = max_pucch_code_rate::dot_35;
  /// Minimum k1 value (distance in slots between PDSCH and HARQ-ACK) that the gNB can use. Values: {1, ..., 15}.
  unsigned min_k1 = 4;

  /// Maximum number of consecutive undecoded PUCCH Format 2 for CSI before an RLF is reported.
  unsigned max_consecutive_kos = 100;
};

/// Parameters that are used to initialize or build the \c PhysicalCellGroupConfig, TS 38.331.
struct phy_cell_group_appconfig {
  /// \brief \c p-NR-FR1, part of \c PhysicalCellGroupConfig, TS 38.331. Values: {-30,...,33}.
  /// The maximum total TX power to be used by the UE in this NR cell group across all serving cells in FR1.
  optional<int> p_nr_fr1;
};

/// Amplitude control application configuration.
struct amplitude_control_appconfig {
  /// Baseband gain back-off. This accounts for the signal PAPR and is applied regardless of clipping settings.
  float gain_backoff_dB = 12.0F;
  /// Power ceiling in dB, relative to the full scale amplitude of the radio.
  float power_ceiling_dBFS = -0.1F;
  /// Clipping of the baseband samples. If enabled, the samples that exceed the power ceiling are clipped.
  bool enable_clipping = false;
};

/// Common uplink parameters of a cell.
struct ul_common_appconfig {
  /// Maximum transmit power allowed in this serving cell. Values: {-30,...,33}dBm.
  optional<int> p_max;
  /// Maximum number of PUCCH grants per slot.
  unsigned max_pucchs_per_slot = 31U;
  /// Maximum number of PUSCH + PUCCH grants per slot.
  unsigned max_ul_grants_per_slot = 32U;
};

struct ssb_appconfig {
  /// SSB period in milliseconds.
  unsigned ssb_period_msec = 10;
  /// \brief \c ss-PBCH-BlockPower, part of \c ServingCellConfigCommonSIB, as per TS 38.331.
  /// Average EPRE of the REs that carry secondary synchronization signals in dBm used for SSB transmission.
  /// Values: {-60,..,70}
  int ssb_block_power = -16;
  /// PSS EPRE to SSS EPRE for SSB, as per TS 38.213, Section 4.1.
  ssb_pss_to_sss_epre pss_to_sss_epre = ssb_pss_to_sss_epre::dB_0;
};

/// Configuration of SIBs and SI-message scheduling.
struct sib_appconfig {
  struct si_sched_info_config {
    /// List of SIB indexes (sib2 => value 2 in list, sib3 => value 3 in list, ...) included in this SI message. The
    /// list has at most 32 elements.
    std::vector<uint8_t> sib_mapping_info;
    /// Periodicity of the SI-message in radio frames. Values: {8, 16, 32, 64, 128, 256, 512}.
    unsigned si_period_rf = 32;
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
};

struct csi_appconfig {
  /// \brief Enable CSI-RS and CSI reporting in the cell.
  bool csi_rs_enabled = true;
  /// \brief \c CSI-RS period in milliseconds. Limited by TS38.214, clause 5.1.6.1.1. Values: {10, 20, 40, 80}.
  unsigned csi_rs_period_msec = 20;
  /// \brief Slot offset for measurement CSI-RS resources. If not set, it is automatically derived to avoid collisions
  /// with SSB and SIB1.
  optional<unsigned> meas_csi_slot_offset;
  /// \brief Slot offset of the first CSI-RS resource used for tracking. If not set, it is automatically derived to
  /// avoid collisions with SSB and SIB1.
  optional<unsigned> tracking_csi_slot_offset;
  /// \brief Slot offset for the zp-CSI-RS resources. If not set, it is automatically derived to avoid collisions with
  /// SSB and SIB1.
  optional<unsigned> zp_csi_slot_offset;
  /// \brief \c powerControlOffset, part of \c NZP-CSI-RS-Resource, as per TS 38.331.
  /// Power offset of PDSCH RE to NZP CSI-RS RE. Value in dB {-8,...,15}.
  int pwr_ctrl_offset = 0;
};

/// MAC Buffer Status Report application configuration.
struct mac_bsr_appconfig {
  /// Periodic Buffer Status Report Timer value in nof. subframes. Values {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160,
  /// 320, 640, 1280, 2560, 0}.
  /// \remark Value 0 equates to periodicBSR-Timer value of infinity.
  unsigned periodic_bsr_timer = 10;
  /// Retransmission Buffer Status Report Timer value in nof. subframes. Values {10, 20, 40, 80, 160, 320, 640, 1280,
  /// 2560, 5120, 10240}.
  unsigned retx_bsr_timer = 80;
  /// Logical Channel SR delay timer in nof. subframes. Values {20, 40, 64, 128, 512, 1024, 2560}.
  optional<unsigned> lc_sr_delay_timer;
};

/// MAC Power Headroom Reporting configuration.
struct mac_phr_appconfig {
  /// \brief \c phr-ProhibitTimer, value in number of subframes for PHR reporting.
  /// Values: {0, 10, 20, 50, 100, 200, 500, 1000}.
  unsigned phr_prohib_timer = 10;
};

/// MAC Scheduler Request configuration.
struct mac_sr_appconfig {
  /// \brief \c sr-ProhibitTimer, or timer for SR transmission on PUCCH.
  /// Values are in ms. Values: {1, 2, 4, 8, 16, 32, 64, 128}. When the field is absent, the UE applies the value 0.
  optional<unsigned> sr_prohibit_timer;
  /// \brief \c sr-TransMax possible values, or maximum number of SR transmissions.
  /// Values: {4, 8, 16, 32, 64}.
  unsigned sr_trans_max = 64;
};

// MAC Cell group application configuration.
struct mac_cell_group_appconfig {
  /// Buffer Status Report configuration.
  mac_bsr_appconfig bsr_cfg;
  /// Power Headroom Reporting configuration.
  mac_phr_appconfig phr_cfg;
  /// Scheduler Request configuration
  mac_sr_appconfig sr_cfg;
};

/// Base cell configuration.
struct base_cell_appconfig {
  /// Physical cell identifier.
  pci_t pci = 1;
  /// Downlink arfcn.
  unsigned dl_arfcn = 536020;
  /// NR band.
  optional<nr_band> band;
  /// Channel bandwidth in MHz.
  bs_channel_bandwidth_fr1 channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
  /// Number of antennas in downlink.
  unsigned nof_antennas_dl = 1;
  /// Number of antennas in uplink.
  unsigned nof_antennas_ul = 1;
  /// Human readable full PLMN (without possible filler digit).
  std::string plmn = "00101";
  /// TAC.
  unsigned tac = 7;
  /// \c q-RxLevMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dBm.
  int q_rx_lev_min = -70;
  /// \c q-QualMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dB.
  int q_qual_min = -20;
  /// SSB parameters.
  ssb_appconfig ssb_cfg;
  /// SIB parameters.
  sib_appconfig sib_cfg;
  /// UL common configuration parameters.
  ul_common_appconfig ul_common_cfg;
  /// PDCCH configuration.
  pdcch_appconfig pdcch_cfg;
  /// PDSCH configuration.
  pdsch_appconfig pdsch_cfg;
  /// PRACH configuration.
  prach_appconfig prach_cfg;
  /// PUSCH configuration.
  pusch_appconfig pusch_cfg;
  /// PUCCH configuration.
  pucch_appconfig pucch_cfg;
  /// Physical Cell Group parameters.
  phy_cell_group_appconfig pcg_cfg;
  /// MAC Cell Gropup parameters.
  mac_cell_group_appconfig mcg_cfg;
  /// Common subcarrier spacing for the entire resource grid. It must be supported by the band SS raster.
  subcarrier_spacing common_scs = subcarrier_spacing::kHz15;
  /// TDD slot configuration.
  optional<tdd_ul_dl_appconfig> tdd_ul_dl_cfg;
  /// Paging configuration.
  paging_appconfig paging_cfg;
  /// CSI configuration.
  csi_appconfig csi_cfg;
};

/// Cell configuration
struct cell_appconfig {
  /// Cell configuration.
  base_cell_appconfig cell;
};

/// RLC UM TX configuration
struct rlc_tx_um_appconfig {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  uint32_t queue_size;      ///< RLC SDU queue size
};

/// RLC UM RX configuration
struct rlc_rx_um_appconfig {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  int32_t  t_reassembly;    ///< Timer used by rx to detect PDU loss (ms)
};

/// RLC UM configuration
struct rlc_um_appconfig {
  rlc_tx_um_appconfig tx;
  rlc_rx_um_appconfig rx;
};

/// RLC UM TX configuration
struct rlc_tx_am_appconfig {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  int32_t  t_poll_retx;     ///< Poll retx timeout (ms)
  uint32_t max_retx_thresh; ///< Max retx threshold
  int32_t  poll_pdu;        ///< Insert poll bit after this many PDUs
  int32_t  poll_byte;       ///< Insert poll bit after this much data (bytes)
  uint32_t max_window = 0;  ///< Custom parameter to limit the maximum window size for memory reasons. 0 means no limit.
  uint32_t queue_size = 4096; ///< RLC SDU queue size
};

/// RLC UM RX configuration
struct rlc_rx_am_appconfig {
  uint16_t sn_field_length;   ///< Number of bits used for sequence number
  int32_t  t_reassembly;      ///< Timer used by rx to detect PDU loss (ms)
  int32_t  t_status_prohibit; ///< Timer used by rx to prohibit tx of status PDU (ms)

  // Implementation-specific parameters that are not specified by 3GPP
  /// Maximum number of visited SNs in the RX window when building a status report. 0 means no limit.
  uint32_t max_sn_per_status = 0;
};

/// RLC AM configuration
struct rlc_am_appconfig {
  rlc_tx_am_appconfig tx;
  rlc_rx_am_appconfig rx;
};

/// RLC configuration
struct rlc_appconfig {
  std::string      mode = "am";
  rlc_um_appconfig um;
  rlc_am_appconfig am;
};

/// F1-U configuration at DU side
struct f1u_du_appconfig {
  int32_t t_notify; ///< Maximum backoff time for transmit/delivery notifications from DU to CU_UP (ms)
};

/// F1-U configuration at CU_UP side
struct f1u_cu_up_appconfig {
  int32_t t_notify; ///< Maximum backoff time for discard notifications from CU_UP to DU (ms)
};

struct pdcp_rx_appconfig {
  uint16_t sn_field_length;       ///< Number of bits used for sequence number
  int32_t  t_reordering;          ///< Timer used to detect PDUs losses (ms)
  bool     out_of_order_delivery; ///< Whether out-of-order delivery to upper layers is enabled
};

struct pdcp_tx_appconfig {
  uint16_t sn_field_length;        ///< Number of bits used for sequence number
  int32_t  discard_timer;          ///< Timer used to notify lower layers to discard PDUs (ms)
  bool     status_report_required; ///< Whether PDCP status report is required
};

struct pdcp_appconfig {
  bool              integrity_protection_required; ///< Whether DRB integrity is required
  pdcp_tx_appconfig tx;
  pdcp_rx_appconfig rx;
};

struct mac_lc_appconfig {
  /// Logical channel priority. Values {1,...,16}. An increasing priority value indicates a lower priority level.
  /// [Implementation-Defined] Only priority > 3 are allowed for DRBs.
  uint8_t priority;
  /// Logical channel group ID. Values {0,...,7}.
  /// [Implementation-Defined] Only LCG ID > 0 are allowed for DRBs.
  uint8_t lc_group_id;
  /// Bucket size duration in milliseconds. Values {5, 10, 20, 50, 100, 150, 300, 500, 1000}. See TS 38.331, \c
  /// bucketSizeDuration.
  unsigned bucket_size_duration_ms;
  /// Prioritized Bit rate value in kiloBytes/s. Values {0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
  /// 32768, 65536, 65537}. Value 65537 means infinity. See TS 38.331, \c prioritisedBitRate.
  unsigned prioritized_bit_rate_kBps;
};

/// QoS configuration
struct srb_appconfig {
  unsigned         srb_id;
  rlc_am_appconfig rlc;
  mac_lc_appconfig mac;
};

/// QoS configuration
struct qos_appconfig {
  five_qi_t           five_qi = uint_to_five_qi(9);
  rlc_appconfig       rlc;
  f1u_du_appconfig    f1u_du;
  f1u_cu_up_appconfig f1u_cu_up;
  pdcp_appconfig      pdcp;
  mac_lc_appconfig    mac;
};

struct amf_appconfig {
  std::string ip_addr                = "127.0.0.1";
  uint16_t    port                   = 38412;
  std::string bind_addr              = "127.0.0.1";
  std::string n2_bind_addr           = "auto";
  std::string n3_bind_addr           = "auto";
  int         sctp_rto_initial       = 120;
  int         sctp_rto_min           = 120;
  int         sctp_rto_max           = 500;
  int         sctp_init_max_attempts = 3;
  int         sctp_max_init_timeo    = 500;
  int         udp_rx_max_msgs        = 256;
  bool        no_core                = false;
};

/// E2 Agent configuration
struct e2_appconfig {
  bool        enable_du_e2           = false;       ///< Whether to enable DU E2 agent
  std::string ip_addr                = "127.0.0.1"; ///< RIC IP address
  uint16_t    port                   = 36421;       ///< RIC port
  std::string bind_addr              = "127.0.0.1"; ///< Local IP address to bind for RIC connection
  int         sctp_rto_initial       = 120;         ///< SCTP initial RTO value for RIC connection
  int         sctp_rto_min           = 120;         ///< SCTP RTO min for RIC connection
  int         sctp_rto_max           = 500;         ///< SCTP RTO max for RIC connection
  int         sctp_init_max_attempts = 3;           ///< SCTP init max attempts for RIC connection
  int         sctp_max_init_timeo    = 500;         ///< SCTP max init timeout for RIC connection
  bool        e2sm_kpm_enabled       = false;       ///< Whether to enable KPM service module
  bool        e2sm_rc_enabled        = false;       ///< Whether to enable RC service module
};

struct cu_cp_neighbor_cell_appconfig_item {
  uint64_t              nr_cell_id;     ///< Cell id.
  std::vector<uint64_t> report_cfg_ids; ///< Report config ids
};

/// \brief Each item describes the relationship between one cell to all other cells.
struct cu_cp_cell_appconfig_item {
  uint64_t    nr_cell_id; ///< Cell id.
  std::string rat = "nr"; ///< RAT of this neighbor cell.

  // TODO: Add optional SSB parameters.
  optional<unsigned> periodic_report_cfg_id;
  optional<unsigned> gnb_id; ///< gNodeB identifier
  optional<nr_band>  band;
  optional<unsigned> ssb_arfcn;
  optional<unsigned> ssb_scs;
  optional<unsigned> ssb_period;
  optional<unsigned> ssb_offset;
  optional<unsigned> ssb_duration;

  std::vector<cu_cp_neighbor_cell_appconfig_item> ncells; ///< Vector of cells that are a neighbor of this cell.
};

/// \brief Report configuration, for now only supporting the A3 event.
struct cu_cp_report_appconfig {
  unsigned           report_cfg_id;
  std::string        report_type;
  optional<unsigned> report_interval_ms;
  std::string        a3_report_type;
  optional<int> a3_offset_db; ///< [-30..30] Note the actual value is field value * 0.5 dB. E.g. putting a value of -6
                              ///< here results in -3dB offset.
  optional<unsigned> a3_hysteresis_db;
  optional<unsigned> a3_time_to_trigger_ms;
};

/// \brief All mobility related configuration parameters.
struct mobility_appconfig {
  std::vector<cu_cp_cell_appconfig_item> cells;          ///< List of all cells known to the CU-CP.
  std::vector<cu_cp_report_appconfig>    report_configs; ///< Report config.
  bool trigger_handover_from_measurements = false;       ///< Whether to start HO if neighbor cell measurements arrive.
};

/// \brief RRC specific configuration parameters.
struct rrc_appconfig {
  bool     force_reestablishment_fallback = false;
  unsigned rrc_procedure_timeout_ms       = 720; ///< Timeout for RRC procedures (2 * default SRB maxRetxThreshold *
                                                 ///< t-PollRetransmit = 2 * 8 * 45ms = 720ms, see TS 38.331 Sec 9.2.1).
};

/// \brief Security configuration parameters.
struct security_appconfig {
  std::string integrity_protection       = "not_needed";
  std::string confidentiality_protection = "required";
  std::string nea_preference_list        = "nea0,nea2,nea1,nea3";
  std::string nia_preference_list        = "nia2,nia1,nia3";
};

struct cu_cp_appconfig {
  int                inactivity_timer           = 7200; // in seconds
  unsigned           ue_context_setup_timeout_s = 3;    // in seconds (must be larger than T310)
  mobility_appconfig mobility_config;
  rrc_appconfig      rrc_config;
  security_appconfig security_config;
};

struct cu_up_appconfig {
  unsigned gtpu_queue_size          = 2048;
  unsigned gtpu_reordering_timer_ms = 0;
  bool     warn_on_drop             = false;
};

/// Configuration of logging functionalities.
struct log_appconfig {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "/tmp/gnb.log";
  /// Default log level for all layers.
  std::string all_level = "warning";
  /// Generic log level assigned to library components without layer-specific level.
  std::string lib_level   = "warning";
  std::string du_level    = "warning";
  std::string cu_level    = "warning";
  std::string phy_level   = "warning";
  std::string radio_level = "info";
  std::string mac_level   = "warning";
  std::string rlc_level   = "warning";
  std::string f1ap_level  = "warning";
  std::string f1u_level   = "warning";
  std::string pdcp_level  = "warning";
  std::string rrc_level   = "warning";
  std::string ngap_level  = "warning";
  std::string sdap_level  = "warning";
  std::string gtpu_level  = "warning";
  std::string sec_level   = "warning";
  std::string fapi_level  = "warning";
  std::string ofh_level   = "warning";
  std::string e2ap_level  = "warning";
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to true to log broadcasting messages and all PRACH opportunities.
  bool broadcast_enabled = false;
  /// Set to a valid file path to print the received symbols.
  std::string phy_rx_symbols_filename;
  /// Set to a valid Rx port number or empty for all ports.
  optional<unsigned> phy_rx_symbols_port = 0;
  /// If true, prints the PRACH frequency-domain symbols.
  bool phy_rx_symbols_prach = false;
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string tracing_filename;
};

struct pcap_appconfig {
  struct {
    std::string filename = "/tmp/gnb_ngap.pcap";
    bool        enabled  = false;
  } ngap;
  struct {
    std::string filename = "/tmp/gnb_e1ap.pcap";
    bool        enabled  = false;
  } e1ap;
  struct {
    std::string filename = "/tmp/gnb_f1ap.pcap";
    bool        enabled  = false;
  } f1ap;
  struct {
    std::string filename = "/tmp/gnb_e2ap.pcap";
    bool        enabled  = false;
  } e2ap;
  struct {
    std::string filename = "/tmp/gnb_gtpu.pcap";
    bool        enabled  = false;
  } gtpu;
  struct {
    std::string filename = "/tmp/gnb_rlc.pcap";
    std::string rb_type  = "all";
    bool        enabled  = false;
  } rlc;
  struct {
    std::string filename = "/tmp/gnb_mac.pcap";
    std::string type     = "udp";
    bool        enabled  = false;
  } mac;
};

/// Metrics report configuration.
struct metrics_appconfig {
  struct {
    unsigned report_period = 0; // RLC report period in ms
    bool     json_enabled  = false;
  } rlc;
  unsigned cu_cp_statistics_report_period = 1; // Statistics report period in seconds
  unsigned cu_up_statistics_report_period = 1; // Statistics report period in seconds
  /// JSON metrics reporting.
  bool        enable_json_metrics      = false;
  std::string addr                     = "127.0.0.1";
  uint16_t    port                     = 55555;
  bool        autostart_stdout_metrics = false;
};

/// Lower physical layer thread profiles.
enum class lower_phy_thread_profile {
  /// Same task worker as the rest of the PHY (ZMQ only).
  blocking = 0,
  /// Single task worker for all the lower physical layer task executors.
  single,
  /// Two task workers - one for the downlink and one for the uplink.
  dual,
  /// Dedicated task workers for each of the subtasks (downlink processing, uplink processing, reception and
  /// transmission).
  quad
};

/// Expert upper physical layer configuration.
struct expert_upper_phy_appconfig {
  /// \brief Sets the maximum allowed downlink processing delay in slots.
  ///
  /// Higher values increase the downlink processing pipeline length, which improves performance and stability for
  /// demanding cell configurations, such as using large bandwidths or higher order MIMO. Higher values also increase
  /// the round trip latency of the radio link.
  unsigned max_processing_delay_slots = 5U;
  /// Number of PUSCH LDPC decoder iterations.
  unsigned pusch_decoder_max_iterations = 6;
  /// Set to true to enable the PUSCH LDPC decoder early stop.
  bool pusch_decoder_early_stop = true;
  /// \brief Selects a PUSCH SINR calculation method.
  ///
  /// Available methods:
  /// -\c channel_estimator: SINR is calculated by the channel estimator using the DM-RS.
  /// -\c post_equalization: SINR is calculated using the post-equalization noise variances of the equalized RE.
  /// -\c evm: SINR is obtained from the EVM of the PUSCH symbols.
  std::string pusch_sinr_calc_method = "post_equalization";
  /// \brief Request headroom size in slots.
  ///
  /// The request headroom size is the number of delayed slots that the upper physical layer will accept, ie, if the
  /// current slot is M, the upper phy will consider the slot M - nof_slots_request_headroom as valid and process it.
  unsigned nof_slots_request_headroom = 0U;
};

struct test_mode_ue_appconfig {
  /// C-RNTI to assign to the test UE.
  rnti_t rnti = rnti_t::INVALID_RNTI;
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
struct test_mode_appconfig {
  /// Creates a UE with the given the given params for testing purposes.
  test_mode_ue_appconfig test_ue;
};

/// Expert SDR Radio Unit configuration.
struct ru_sdr_expert_appconfig {
  /// System time-based throttling. See \ref lower_phy_configuration::system_time_throttling for more information.
  float lphy_dl_throttling = 0.0F;
  /// \brief Enables discontinuous transmission mode for the radio front-ends supporting it.
  ///
  /// Discontinuous Transmission (DTX) is a power-saving technique used in radio communication where the transmitter is
  /// turned off during periods of silence or when no data needs to be transmitted. This flag allows the user to
  /// activate DTX for radio front-ends that support this transmission mode.
  ///
  /// When DTX is enabled, the radio transmitter intelligently manages its transmission state, reducing power
  /// consumption during idle or silent periods. This is particularly beneficial in scenarios where power efficiency is
  /// a critical consideration, such as battery-operated devices.
  bool discontinuous_tx_mode = false;
  /// \brief Power ramping time of the transmit chain in microseconds.
  ///
  /// This parameter represents the duration, in microseconds, required for the transmit chain to reach its full power
  /// level.
  ///
  /// In discontinuous transmission mode, the transmitter is powered on ahead of the actual data transmission. By doing
  /// so, the data-carrying samples remain unaffected by any transient effects or fluctuations in the transmit chain
  /// during the power ramping time. The maximum supported power ramping time is equivalent to the duration of two NR
  /// slots.
  ///
  /// \note It is recommended to configure this parameter carefully, taking into account the characteristics of the
  /// transmit chain in order to achieve optimal performance.
  /// \note Powering up the transmitter ahead of time requires starting the transmission earlier, and reduces the time
  /// window for the radio to transmit the provided samples.
  float power_ramping_time_us = 0.0F;
};

/// gNB app SDR Radio Unit configuration.
struct ru_sdr_appconfig {
  /// Sampling frequency in MHz.
  double srate_MHz = 61.44;
  /// RF driver name.
  std::string device_driver = "uhd";
  /// RF driver arguments.
  std::string device_arguments = "";
  /// All transmit channel gain in decibels.
  double tx_gain_dB = 50.0;
  /// All receive channel gain in decibels.
  double rx_gain_dB = 60.0;
  /// Center frequency offset in hertz applied to all radio channels.
  double center_freq_offset_Hz = 0.0;
  /// Clock calibration in Parts Per Million (PPM). It is applied to the carrier frequency.
  double calibrate_clock_ppm = 0.0;
  /// LO Offset in MHz. It shifts the LO from the center frequency for moving the LO leakage out of the channel.
  double lo_offset_MHz = 0.0;
  /// \brief Rx to Tx radio time alignment calibration in samples.
  ///
  /// Compensates for the reception and transmission time misalignment inherent to the RF device. Setting this parameter
  /// overrides the default calibration, which is dependent on the selected RF device driver. Positive values reduce the
  /// RF transmission delay with respect to the RF reception. Since the UE receives the DL signal earlier, an incoming
  /// PRACH will also be detected earlier within the reception window. Negative values have the opposite effect, for
  /// example, a value of -1000 at a sample rate of 61.44 MHz increases the transmission delay and causes an incoming
  /// PRACH to be detected 16.3 us later within the reception window.
  optional<int> time_alignment_calibration;
  /// Synchronization source.
  std::string synch_source = "default";
  /// Clock source.
  std::string clock_source = "default";
  /// Over-the wire format. Determines the format in which samples are transported from the radio to the host.
  std::string otw_format = "default";
  /// Expert SDR Radio Unit settings.
  ru_sdr_expert_appconfig expert_cfg;
  /// Amplitude control configuration.
  amplitude_control_appconfig amplitude_cfg;
};

/// gNB app Open Fronthaul base cell configuration.
struct ru_ofh_base_cell_appconfig {
  /// \brief RU operating bandwidth.
  ///
  /// Set this option when the operating bandwidth of the RU is larger than the configured bandwidth of the cell.
  optional<bs_channel_bandwidth_fr1> ru_operating_bw;
  /// T1a maximum parameter for downlink Control-Plane in microseconds.
  unsigned T1a_max_cp_dl = 500U;
  /// T1a minimum parameter for downlink Control-Plane in microseconds.
  unsigned T1a_min_cp_dl = 258U;
  /// T1a maximum parameter for uplink Control-Plane in microseconds.
  unsigned T1a_max_cp_ul = 500U;
  /// T1a minimum parameter for uplink Control-Plane in microseconds.
  unsigned T1a_min_cp_ul = 285U;
  /// T1a maximum parameter for downlink User-Plane in microseconds.
  unsigned T1a_max_up = 300U;
  /// T1a minimum parameter for downlink User-Plane in microseconds.
  unsigned T1a_min_up = 85U;
  /// Ta4 maximum parameter for uplink User-Plane in microseconds.
  unsigned Ta4_max = 300U;
  /// Ta4 minimum parameter for uplink User-Plane in microseconds.
  unsigned Ta4_min = 85U;
  /// Enables the Control-Plane PRACH message signalling.
  bool is_prach_control_plane_enabled = true;
  /// \brief Downlink broadcast flag.
  ///
  /// If enabled, broadcasts the contents of a single antenna port to all downlink RU eAxCs.
  bool is_downlink_broadcast_enabled = false;
  /// If set to true, the payload size encoded in a eCPRI header is ignored.
  bool ignore_ecpri_payload_size_field = false;
  /// If set to true, the sequence id encoded in a eCPRI packet is ignored.
  bool ignore_ecpri_seq_id_field = false;
  /// Uplink compression method.
  std::string compression_method_ul = "bfp";
  /// Uplink compression bitwidth.
  unsigned compression_bitwidth_ul = 9;
  /// Downlink compression method.
  std::string compression_method_dl = "bfp";
  /// Downlink compression bitwidth.
  unsigned compression_bitwidth_dl = 9;
  /// PRACH compression method.
  std::string compression_method_prach = "bfp";
  /// PRACH compression bitwidth.
  unsigned compression_bitwidth_prach = 9;
  /// Downlink static compression header flag.
  bool is_downlink_static_comp_hdr_enabled = true;
  /// Uplink static compression header flag.
  bool is_uplink_static_comp_hdr_enabled = true;
  /// IQ data scaling to be applied prior to Downlink data compression.
  float iq_scaling = 0.35F;
};

/// gNB app Open Fronthaul cell configuration.
struct ru_ofh_cell_appconfig {
  /// Base cell configuration.
  ru_ofh_base_cell_appconfig cell;
  /// Ethernet network interface name.
  std::string network_interface = "enp1s0f0";
  /// Promiscuous mode flag.
  bool enable_promiscuous_mode = true;
  /// MTU size.
  units::bytes mtu_size{9000};
  /// Radio Unit MAC address.
  std::string ru_mac_address = "70:b3:d5:e1:5b:06";
  /// Distributed Unit MAC address.
  std::string du_mac_address = "00:11:22:33:00:77";
  /// V-LAN Tag control information field.
  uint16_t vlan_tag = 1U;
  /// RU PRACH port.
  std::vector<unsigned> ru_prach_port_id = {4, 5};
  /// RU Downlink port.
  std::vector<unsigned> ru_dl_port_id = {0, 1};
  /// RU Uplink port.
  std::vector<unsigned> ru_ul_port_id = {0, 1};
};

/// gNB app Open Fronthaul Radio Unit configuration.
struct ru_ofh_appconfig {
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha = 0;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta = 0;
  /// Downlink processing time in microseconds.
  unsigned dl_processing_time = 400U;
  /// Base cell configuration for the Radio Unit.
  ru_ofh_base_cell_appconfig base_cell_cfg;
  /// Individual Open Fronthaul cells configurations.
  std::vector<ru_ofh_cell_appconfig> cells = {{}};
};

struct buffer_pool_appconfig {
  std::size_t nof_segments = 1048576;
  std::size_t segment_size = byte_buffer_segment_pool_default_segment_size();
};

/// CPU isolation configuration in the gNB app.
struct cpu_isolation_config {
  /// Set of CPUs exclusively used by the gNB app.
  std::string isolated_cpus;
  /// Set of CPUs dedicated to other operating system processes.
  std::string os_tasks_cpus;
};

/// CPU affinities configuration for the gNB app.
struct cpu_affinities_appconfig {
  /// L1 uplink CPU affinity mask.
  gnb_os_sched_affinity_config l1_ul_cpu_cfg;
  /// L1 downlink workers CPU affinity mask.
  gnb_os_sched_affinity_config l1_dl_cpu_cfg;
  /// L2 workers CPU affinity mask.
  gnb_os_sched_affinity_config l2_cell_cpu_cfg;
  /// Radio Unit workers CPU affinity mask.
  gnb_os_sched_affinity_config ru_cpu_cfg;
  /// Low priority workers CPU affinity mask.
  gnb_os_sched_affinity_config low_priority_cpu_cfg;
  /// CPUs isolation.
  optional<cpu_isolation_config> isol_cpus;
};

/// Upper PHY thread configuration for the gNB.
struct upper_phy_threads_appconfig {
  /// \brief PDSCH processor type.
  ///
  /// Use of there options:
  /// - \c automatic: selects \c lite implementation if \c nof_pdsch_threads is one, otherwise \c concurrent, or
  /// - \c generic: for using unoptimized PDSCH processing, or
  /// - \c concurrent: for using a processor that processes code blocks in parallel, or
  /// - \c lite: for using a memory optimized processor.
  std::string pdsch_processor_type = "auto";
  /// \brief Number of threads for concurrent PUSCH decoding.
  ///
  /// If the number of PUSCH decoder threads is greater than zero, the PUSCH decoder will enqueue received soft bits and
  /// process them asynchronously. Otherwise, PUSCH decoding will be performed synchronously.
  ///
  /// In non-real-time operations (e.g., when using ZeroMQ), setting this parameter to a non-zero value can potentially
  /// introduce delays in uplink HARQ feedback.
  unsigned nof_pusch_decoder_threads = 0;
  /// Number of threads for processing PUSCH and PUCCH.
  unsigned nof_ul_threads = 1;
  /// Number of threads for processing PDSCH, PDCCH, NZP CSI-RS and SSB. It is set to 1 by default.
  unsigned nof_dl_threads = 1;
};

/// Lower PHY thread configuration fo the gNB.
struct lower_phy_threads_appconfig {
  /// \brief Lower physical layer thread profile.
  ///
  /// If not configured, a default value is selected based on the number of available CPU cores.
  lower_phy_thread_profile execution_profile;
};

/// Open Fronthaul thread configuration for the gNB.
struct ofh_threads_appconfig {
  bool is_downlink_parallelized = true;
};

/// Expert threads configuration of the gNB app.
struct expert_threads_appconfig {
  expert_threads_appconfig()
  {
    unsigned nof_threads = compute_host_nof_hardware_threads();

    if (nof_threads < 4) {
      upper_threads.nof_ul_threads            = 1;
      upper_threads.nof_pusch_decoder_threads = 0;
      upper_threads.nof_dl_threads            = 2;
      lower_threads.execution_profile         = lower_phy_thread_profile::single;
      ofh_threads.is_downlink_parallelized    = false;
    } else if (nof_threads < 8) {
      upper_threads.nof_ul_threads            = 1;
      upper_threads.nof_pusch_decoder_threads = 1;
      upper_threads.nof_dl_threads            = 4;
      lower_threads.execution_profile         = lower_phy_thread_profile::dual;
      ofh_threads.is_downlink_parallelized    = true;
    } else if (nof_threads < 16) {
      upper_threads.nof_ul_threads            = 1;
      upper_threads.nof_pusch_decoder_threads = 1;
      upper_threads.nof_dl_threads            = 4;
      lower_threads.execution_profile         = lower_phy_thread_profile::quad;
      ofh_threads.is_downlink_parallelized    = true;
    } else {
      upper_threads.nof_ul_threads            = 2;
      upper_threads.nof_pusch_decoder_threads = 2;
      upper_threads.nof_dl_threads            = 6;
      lower_threads.execution_profile         = lower_phy_thread_profile::quad;
      ofh_threads.is_downlink_parallelized    = true;
    }
  }

  /// Upper PHY thread configuration of the gNB app.
  upper_phy_threads_appconfig upper_threads;
  /// Lower PHY thread configuration of the gNB app.
  lower_phy_threads_appconfig lower_threads;
  /// Open Fronthaul thread configuration of the gNB app.
  ofh_threads_appconfig ofh_threads;
};

/// Expert configuration of the gNB app.
struct expert_execution_appconfig {
  /// CPU affinities of the gNB app.
  cpu_affinities_appconfig affinities;
  /// Expert thread configuration of the gNB app.
  expert_threads_appconfig threads;
};

/// HAL configuration of the gNB app.
struct hal_appconfig {
  /// EAL configuration arguments.
  std::string eal_args;
};

/// Monolithic gnb application configuration.
struct gnb_appconfig {
  /// Logging configuration.
  log_appconfig log_cfg;
  /// PCAP configuration.
  pcap_appconfig pcap_cfg;
  /// Metrics configuration.
  metrics_appconfig metrics_cfg;
  /// gNodeB identifier.
  uint32_t gnb_id = 411;
  /// Length of gNB identity in bits. Values {22,...,32}.
  uint8_t gnb_id_bit_length = 32;
  /// Node name.
  std::string ran_node_name = "srsgnb01";
  /// AMF configuration.
  amf_appconfig amf_cfg;
  /// CU-CP configuration.
  cu_cp_appconfig cu_cp_cfg;
  /// CU-CP configuration.
  cu_up_appconfig cu_up_cfg;
  /// \brief E2 configuration.
  e2_appconfig e2_cfg;
  /// Radio Unit configuration.
  variant<ru_sdr_appconfig, ru_ofh_appconfig> ru_cfg = {ru_sdr_appconfig{}};
  /// \brief Cell configuration.
  ///
  /// \note Add one cell by default.
  std::vector<cell_appconfig> cells_cfg = {{}};

  /// \brief QoS configuration.
  std::vector<qos_appconfig> qos_cfg;

  /// \brief QoS configuration.
  std::map<srb_id_t, srb_appconfig> srb_cfg;

  /// \brief Network slice configuration.
  std::vector<s_nssai_t> slice_cfg = {s_nssai_t{1}};

  /// Expert physical layer configuration.
  expert_upper_phy_appconfig expert_phy_cfg;

  /// Configuration for testing purposes.
  test_mode_appconfig test_mode_cfg = {};

  /// \brief NTN configuration.
  optional<ntn_config> ntn_cfg;

  /// \brief Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;

  /// \brief Expert configuration.
  expert_execution_appconfig expert_execution_cfg;

  /// \brief HAL configuration.
  optional<hal_appconfig> hal_config;
};

/// \brief Monolithic gnb parsed application configuration.
///
/// Parsed configuration includes the common cell support.
struct gnb_parsed_appconfig {
  /// gNB application configuration.
  gnb_appconfig config;
  /// \brief Base cell application configuration.
  ///
  /// \note When a cell is added, it will use the values of this base cell as default values for its base cell
  /// configuration. This parameter usage is restricted for filling cell information in the \remark cell_cfg variable.
  base_cell_appconfig common_cell_cfg;
};

} // namespace srsran
