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

#include "sched_consts.h"
#include "scheduler_dci.h"
#include "vrb_alloc.h"
#include "srsran/adt/static_vector.h"
#include "srsran/mac/lcid_dl_sch.h"
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_rs/csi_rs_types.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/precoding/precoding_constants.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sch/modulation_scheme.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/uci/uci_configuration.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/dmrs.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/scheduler_pucch_format.h"
#include <cstddef>

namespace srsran {

/// The precoding information associated with PDCCH PDUs.
struct pdcch_precoding_info {};

/// The precoding information associated with PDSCH PDUs.
struct pdsch_precoding_info {
  /// Precoding Resource Block Group (PRG) information.
  using prg_info = csi_report_pmi;

  /// \brief Size in RBs of a precoding resource block group (PRG) to which same precoding and digital beamforming gets
  /// applied. Values: {1,...,275}.
  unsigned nof_rbs_per_prg;
  /// PRG list.
  static_vector<prg_info, precoding_constants::MAX_NOF_PRG> prg_infos;
};

/// Transmit power information associated with PDCCH PDU.
struct tx_power_pdcch_information {
  /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE. See 3GPP TS 38.214, clause 5.2.2.3.1. Values {-3, 0, 3, 6} dB.
  /// \remark If the UE has not been provided dedicated higher layer parameters, the UE may assume that the ratio of
  /// PDCCH DMRS EPRE to SSS EPRE is within -8 dB and 8 dB when the UE monitors PDCCHs for a DCI format 1_0 with CRC
  /// scrambled by SI-RNTI, P-RNTI, or RA-RNTI. See TS 38.213, clause 4.1.
  /// \remark [Implementation-defined] In case UE is not configured with powerControlOffsetSS we assume it to be 0dB.
  int8_t pwr_ctrl_offset_ss{0};
};

struct dmrs_information {
  /// Bitmap of DM-RS position symbols.
  dmrs_symbol_mask dmrs_symb_pos;
  dmrs_config_type config_type;
  /// \brief DMRS-Scrambling-ID (see TS 38.211 sec 7.4.1.1.1) as provided by parameter \f$N^{n_{SCID}}_{ID}\f$.
  /// Values: (0..65535).
  unsigned dmrs_scrambling_id;
  /// PHY shall disregard this parameter if lowPaprDmrs=0.
  unsigned dmrs_scrambling_id_complement;
  /// False means that dmrs_scrambling_id == dmrs_scrambling_id_complement.
  bool low_papr_dmrs;
  /// \brief DMRS sequence initialization (see TS 38.211 sec 7.4.1.1.2), as provided by parameter n_{SCID}.
  /// Values: false -> 0, true -> 1.
  bool n_scid;
  /// Values: (1..3).
  uint8_t num_dmrs_cdm_grps_no_data;
  /// \brief Bitmap of antenna ports. Bit 0 corresponds to antenna port 1000 and bit 11 to antenna port 1011, and
  /// each bit=1 mean DM-RS port used.
  bounded_bitset<12> dmrs_ports;
};

struct dci_context_information {
  const bwp_configuration*     bwp_cfg;
  const coreset_configuration* coreset_cfg;
  /// RNTI used to identify the destination of this DCI (e.g. UE, RA-RNTI, SI, Paging).
  rnti_t rnti;
  /// Parameter \f$n_{ID}\f$ used for PDCCH Data scrambling as per 3GPP TS 38.211 [2], sec 7.3.2.3. Values: (0..65535).
  /// For a UE-specific search space it equals the higherlayer parameter PDCCH-DMRS-Scrambling-ID if configured,
  /// otherwise it should be set to the phy cell ID.
  unsigned n_id_pdcch_data;
  /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling, as per 3GPP TS 38.211 [2], sec 7.3.2.3.
  /// Values: (0..65535). For a UE-specific search space where PDCCH-DMRSScrambling-ID is configured, this param
  /// equals the CRNTI. Otherwise, it should be set to 0.
  unsigned n_rnti_pdcch_data;
  /// CCE position of the allocated PDCCH.
  cce_position cces;
  /// Starting symbol of the Search Space.
  unsigned starting_symbol;
  /// Precoding info used for this DCI. This field is empty in case of 1 antenna port.
  optional<pdcch_precoding_info> precoding_info;
  /// Transmission power information used for this DCI.
  tx_power_pdcch_information tx_pwr;
  /// Parameter \f$N_{ID}\f$ used for PDCCH DMRS scrambling as per TS38.211, 7.4.1.3.1. Values: {0, ..., 65535}.
  unsigned n_id_pdcch_dmrs;

  /// \brief Information relative to a PDCCH allocation decision that is used for the purpose of logging or
  /// tracing, but not passed to the PHY.
  struct decision_context {
    /// Chosen SearchSpace-Id.
    search_space_id ss_id;
    /// DCI format string.
    const char* dci_format;
    /// Number of slots the UE is expected to wait before transmitting a DL HARQ-ACK, upon a PDSCH reception.
    optional<unsigned> harq_feedback_timing;
  } context;
};

/// PDCCH DL allocation.
struct pdcch_dl_information {
  /// Context associated with PDCCH allocation.
  dci_context_information ctx;
  /// DL DCI unpacked content.
  dci_dl_info dci;
};

/// PDCCH UL allocation.
struct pdcch_ul_information {
  /// Context associated with PDCCH allocation.
  dci_context_information ctx;
  /// UL DCI unpacked content.
  dci_ul_info dci;
};

/// PDSCH codeword.
struct pdsch_codeword {
  /// Modulation and coding scheme.
  sch_mcs_description mcs_descr;
  /// \brief MCS index, range {0, ..., 31} (See TS38.214 Section 5.1.3.1).
  /// \note Should match value sent in DCI.
  sch_mcs_index mcs_index;
  /// MCS table (See TS38.214 Section 5.1.3.1).
  pdsch_mcs_table mcs_table;
  /// Redundancy version index (see TS38.212 Table 5.4.2.1-2, and TS38.214 Table 5.1.2.1-2).
  uint8_t rv_index;
  /// Transport block size, in bytes (see TS38.214 Section 5.1.3.2).
  uint32_t tb_size_bytes;
  /// Whether this is the first Tx or retx of the HARQ codeword.
  bool new_data;
};

/// Transmit power information associated with PDSCH PDU.
struct tx_power_pdsch_information {
  /// Ratio of PDSCH EPRE to NZP CSI-RS EPRE when UE derives CSI feedback. See 3GPP TS 38.214, clause 5.2.2.3.1. Values
  /// {-8,...,15} dB with 1 dB step size.
  /// \remark [Implementation-defined] In case UE is not configured with powerControlOffset we assume it to be 0dB.
  int8_t pwr_ctrl_offset{0};
  /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE. See 3GPP TS 38.214, clause 5.2.2.3.1. Values {-3, 0, 3, 6} dB.
  /// \remark [Implementation-defined] In case UE is not configured with powerControlOffsetSS we assume it to be 0dB.
  int8_t pwr_ctrl_offset_ss{0};
};

/// \brief Information relative to a PDSCH grant in a given slot.
struct pdsch_information {
  rnti_t                                                 rnti;
  const bwp_configuration*                               bwp_cfg;
  const coreset_configuration*                           coreset_cfg;
  vrb_alloc                                              rbs;
  ofdm_symbol_range                                      symbols;
  static_vector<pdsch_codeword, MAX_CODEWORDS_PER_PDSCH> codewords;
  dmrs_information                                       dmrs;
  /// Parameter n_ID, used for scrambling, as per TS 38.211, Section 7.3.1.1.
  unsigned n_id;
  /// Number of layers as per TS 38.211, Section 7.3.1.3. Values: {1,...,8}.
  unsigned nof_layers;
  /// Whether the PDSCH is interleaved via VRB-to-PRB mapping.
  bool                  is_interleaved;
  search_space_set_type ss_set_type;
  dci_dl_format         dci_fmt;
  /// HARQ process number as per TS38.212 Section 7.3.1.1. Values: {0,...,15}.
  harq_id_t harq_id;
  /// Precoding information for the PDSCH. This field is empty in case of 1-antenna port setups.
  optional<pdsch_precoding_info> precoding;
  /// Transmit power information for the PDSCH.
  tx_power_pdsch_information tx_pwr_info;
};

/// Dummy MAC CE payload.
/// To be replaced by other MAC CE payload when its supported.
using dummy_ce_payload = unsigned;

/// Timing Advance Command CE payload.
struct ta_cmd_ce_payload {
  uint8_t  tag_id;
  unsigned ta_cmd;
};

struct dl_msg_lc_info {
  /// Values of LCID for DL-SCH. See TS 38.321, Table 6.2.1-1.
  lcid_dl_sch_t lcid;
  /// Number of scheduled bytes for this specific logical channel. {0..65535}.
  unsigned sched_bytes;
  /// Holds payload of CE except UE Contention Resolution Identity.
  variant<ta_cmd_ce_payload, dummy_ce_payload> ce_payload;
};

struct dl_msg_tb_info {
  /// List of logical channels to schedule in this TB.
  static_vector<dl_msg_lc_info, MAX_LC_PER_TB> lc_chs_to_sched;
};

/// Dedicated DL Grant for UEs.
struct dl_msg_alloc {
  pdsch_information                             pdsch_cfg;
  static_vector<dl_msg_tb_info, MAX_NOF_LAYERS> tb_list;

  /// \brief Information relative to a PDSCH allocation decision that is used for the purpose of logging or
  /// tracing, but not passed to the PHY.
  struct decision_context {
    /// UE index of allocated UE.
    du_ue_index_t ue_index;
    /// Chosen k1 delay to receive UCI HARQ-ACK.
    unsigned k1;
    /// Chosen search space id
    search_space_id ss_id;
    /// Number of times the HARQ process has been retransmitted.
    unsigned nof_retxs;
    /// Offset that the OLLA algorithm applied to the DL MCS candidate to account for channel impairments.
    optional<float> olla_offset;
  } context;
};

struct pusch_information {
  rnti_t                   rnti;
  const bwp_configuration* bwp_cfg;
  vrb_alloc                rbs;
  ofdm_symbol_range        symbols;
  /// \brief For resource allocation type 1, it indicates if intra-slot frequency hopping is enabled, as per TS38.212
  /// Section 7.3.1.1.
  bool intra_slot_freq_hopping;
  /// \brief Index of the first PRB after intra-slot frequency hopping, as indicated by the value of \f$RB_{start}\f$
  /// for i=1, as per TS38.214, Section 6.3. Valid when intra_slot_freq_hopping is enabled. Values: {0,...274}.
  uint16_t pusch_second_hop_prb;
  /// \brief The uplink Tx Direct Current location for the carrier. Only values in the value range of this field
  /// between 0 and 3299, which indicate the subcarrier index within the carrier corresponding to the numerology of the
  /// corresponding uplink BWP and value 3300, which indicates "Outside the carrier" and value 3301, which indicates
  /// "Undetermined position within the carrier" are used.
  uint16_t tx_direct_current_location;
  /// Indicates whether there is 7.5 kHz shift or not.
  bool ul_freq_shift_7p5khz;
  /// Modulation and coding scheme.
  sch_mcs_description mcs_descr;
  /// \brief MCS index, range {0, ..., 31} (See TS38.214 Section 5.1.4.1).
  /// \note Should match value sent in DCI.
  sch_mcs_index mcs_index;
  /// MCS table (See TS38.214 Section 6.1.4.1).
  pusch_mcs_table mcs_table;
  /// Indicates if transform precoding is enabled or disabled (see TS 38.214, Section 6.1.4.1).
  bool transform_precoding;
  /// Parameter \f$n_{ID}\f$ as per TS38.211 Section 6.3.1.1. Values: {0,...,1023}.
  uint16_t n_id;
  /// Number of layers as per TS38.211, Section 6.3.1.3.
  unsigned nof_layers;
  /// DMRS configuration as per TS38.211 Section 6.4.1.1.
  dmrs_information dmrs;
  /// \brief PUSCH DMRS ID \f$n_{ID}^{RS}\f$ as per TS38.211 Section 6.4.1.1.1.2. This field is only valid when
  /// transform_precoding is enabled. Values: {0,...,1007} (Rel-15), {0,...,65535} (Rel-16).
  uint16_t pusch_dmrs_id;
  /// \brief PUSCH DMRS hopping mode as per TS38.211, Section 6.4.1.1.1.2. It is only valid when the transform
  /// precoding for PUSCH is enabled.
  enum class dmrs_hopping_mode { no_hopping, group_hopping, sequence_hopping } dmrs_hopping_mode;
  /// Redundancy version index (see TS38.214 Table 6.1.4). Values: {0,...,3}.
  uint8_t rv_index;
  /// HARQ process number as per TS38.212 Section 6.3.1.1. Values: {0,...,15}.
  uint8_t harq_id;
  /// \brief Signals whether the PUSCH PDU corresponds to an initial transmission or a retransmission of a MAC PDU for
  /// this HARQ process ID for this TB. Note: Unlike NDI, new_data does not toggle to indicate new transmission,
  /// but is set to 1.
  bool new_data;
  /// Transport block size in bytes.
  uint32_t tb_size_bytes;
  /// \brief Number of CBs in the TB (could be more than the number of CBs in this PUSCH transmission). Should be set
  /// to zero in any of the following conditions: 1) CBG is not supported or requested 2) newData=1 (new transmission)
  /// 3) tbSize=0.
  uint16_t num_cb;
};

struct uci_info {
  /// Contains the HARQ-ACK information for UCI on PUSCH.
  struct harq_info {
    /// Number of bits of ACK to be reported.
    uint16_t harq_ack_nof_bits = 0;
    /// \f$\beta^{HARQ-ACK}_{offset}\f$ parameter, as per Section 9.3, TS 38.213. The default value is defined in \c
    /// BetaOffsets, TS 38.331.
    uint8_t beta_offset_harq_ack = 11;
  };

  /// Contains the CSI part 1 and part 2 information for UCI on PUSCH.
  struct csi_info {
    /// Contains information how the CSI bits are to be decoded.
    csi_report_configuration csi_rep_cfg;
    /// Number of bits of CSI Part 1 to be reported.
    uint16_t csi_part1_nof_bits = 0;
    /// \f$\beta^{CSI-1}_{offset}\f$ parameter, as per Section 9.3, TS 38.213. The default value is defined in \c
    /// BetaOffsets, TS 38.331.
    uint8_t beta_offset_csi_1 = 13;
    /// \f$\beta^{CSI-2}_{offset}\f$ parameter, as per Section 9.3, TS 38.213.
    /// If set, the CSI report includes CSI Part 2.
    optional<uint8_t> beta_offset_csi_2;
  };

  optional<harq_info> harq;
  optional<csi_info>  csi;
  /// \f$\alpha\f$ parameter, as per Section 6.3.2.4.1.1-3, TS 38.212.
  alpha_scaling_opt alpha;
};

/// \brief RAR grant composed of subheader as per TS38.321 6.2.2, payload as per TS38.321 6.2.3,
/// with UL grant as per TS38.213, Table 8.2-1.
struct rar_ul_grant {
  // MAC subheader.
  uint16_t rapid;

  // RAR payload.
  uint16_t ta;
  rnti_t   temp_crnti;

  // UL Grant Payload.
  bool          freq_hop_flag;
  uint8_t       time_resource_assignment;
  uint16_t      freq_resource_assignment;
  sch_mcs_index mcs;
  int8_t        tpc;
  bool          csi_req;
};

/// Stores the information associated with a RAR.
struct rar_information {
  pdsch_information                                  pdsch_cfg;
  static_vector<rar_ul_grant, MAX_RAR_PDUS_PER_SLOT> grants;
};

/// Stores the information associated with an SSB.
struct ssb_information {
  unsigned          ssb_index;
  crb_interval      crbs;
  ofdm_symbol_range symbols;
};

/// Stores the information associated with an SIB1 or other SI allocation.
struct sib_information {
  enum si_indicator_type { sib1, other_si } si_indicator;
  optional<uint8_t> si_msg_index;
  unsigned          nof_txs;
  pdsch_information pdsch_cfg;
};

/// See ORAN WG8, 9.2.3.3.12 - Downlink Broadcast Allocation.
struct dl_broadcast_allocation {
  static_vector<ssb_information, MAX_SSB_PER_SLOT>     ssb_info;
  static_vector<sib_information, MAX_SI_PDUS_PER_SLOT> sibs;
};

struct paging_ue_info {
  /// Type of Paging. RAN initiated or CN initiated.
  enum paging_identity_type { ran_ue_paging_identity, cn_ue_paging_identity } paging_type_indicator;
  /// Paging identity assigned to UE. Possible values are \c I-RNTI-Value (Bit string of size 40) and \c NG-5G-S-TMSI
  /// (Bit string of size 48). See TS 38.331.
  uint64_t paging_identity;
};

/// Stores the information associated with Paging allocation.
struct dl_paging_allocation {
  static_vector<paging_ue_info, MAX_PAGING_RECORDS_PER_PAGING_PDU> paging_ue_list;
  pdsch_information                                                pdsch_cfg;
};

/// Stores the information associated with a CSI-RS signalling.
struct csi_rs_info {
  const bwp_configuration* bwp_cfg;
  /// Range of RBs where this CSI resource starts and ends, with relation to CRB#0. Only multiples of 4 are allowed.
  crb_interval crbs;
  csi_rs_type  type;
  /// \brief Row entry into the CSI Resource location table, as per 3GP TS 38.211, sec 7.4.1.5.3 and table 7.4.1.5.3-1.
  /// Values: {1,...,18}.
  uint8_t row;
  /// \brief Bitmap defining the frequencyDomainAllocation as per 3GPP TS 38.211, sec 7.4.1.5.3 and 3GPP TS 38.331
  /// "CSIResource Mapping".
  bounded_bitset<12, false> freq_domain;
  /// \brief The time domain location l0 and firstOFDMSymbolInTimeDomain as per 3GPP TS 38.211, sec 7.4.1.5.3.
  /// Values: {0,...,13}.
  uint8_t symbol0;
  /// \brief The time domain location l1 and firstOFDMSymbolInTimeDomain2 as per 3GPP TS 38.211, sec 7.4.1.5.3.
  uint8_t                  symbol1;
  csi_rs_cdm_type          cdm_type;
  csi_rs_freq_density_type freq_density;
  /// \brief ScramblingID of the CSI-RS as per 3GPP TS 38.214, sec 5.2.2.3.1. Values: {0,...,1023}.
  uint16_t scrambling_id;
  /// Ratio of PDSCH EPRE to NZP CSI-RS EPRE as per 3GPP TS 38.214, clause 5.2.2.3.1. Values: {-8,...,15}.
  int8_t power_ctrl_offset;
  /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE. Values: {-3,0,3,6}.
  int8_t power_ctrl_offset_ss;
};

struct dl_sched_result {
  /// Number of DL symbols active for this slot.
  unsigned nof_dl_symbols;

  /// Allocated DL PDCCHs. Includes both SIB, RAR and Data PDCCHs.
  static_vector<pdcch_dl_information, MAX_DL_PDCCH_PDUS_PER_SLOT> dl_pdcchs;

  /// Allocated UL PDCCHs.
  static_vector<pdcch_ul_information, MAX_UL_PDCCH_PDUS_PER_SLOT> ul_pdcchs;

  /// Allocation of SSB and SIBs.
  dl_broadcast_allocation bc;

  /// Allocation of dedicated RARs.
  static_vector<rar_information, MAX_RAR_PDUS_PER_SLOT> rar_grants;

  /// Allocation of Paging messages.
  static_vector<dl_paging_allocation, MAX_PAGING_PDUS_PER_SLOT> paging_grants;

  /// Allocation of dedicated UE messages.
  static_vector<dl_msg_alloc, MAX_UE_PDUS_PER_SLOT> ue_grants;

  /// Allocation of CSI-RS messages.
  static_vector<csi_rs_info, MAX_CSI_RS_PDUS_PER_SLOT> csi_rs;
};

struct ul_sched_info {
  pusch_information  pusch_cfg;
  optional<uci_info> uci;

  /// \brief Information relative to a PDSCH allocation decision that is used for the purpose of logging or
  /// tracing, but not passed to the PHY.
  struct decision_context {
    du_ue_index_t   ue_index;
    search_space_id ss_id;
    /// Chosen k2 delay between UL PDCCH and PUSCH.
    unsigned k2;
    /// Number of times the HARQ process has been retransmitted.
    unsigned nof_retxs;
    /// Delay between PDSCH message with RAR and its corresponding PUSCH.
    optional<unsigned> msg3_delay;
    /// Offset that the OLLA algorithm applied to derive the UL MCS.
    optional<float> olla_offset;
  } context;
};

struct prach_occasion_info {
  /// Physical Cell identifier.
  pci_t pci;
  /// Number of time-domain PRACH occasions (\f$N^{RAslot}_t\f$), as per TS38.211 Tables 6.3.3.2-[2-4].
  uint8_t nof_prach_occasions;
  /// RACH format information for the PRACH occasions.
  prach_format_type format;
  /// Frequency domain occasion index \f$n \in \{0,...,M-1\}\f$, where \f$M\f$ is the higher-layer parameter msg1-FDM,
  /// which can take the values \f$\{1,2,4,8\}\f$. See TS38.211, sec 6.3.3.2. Possible values {0,...,7}.
  uint8_t index_fd_ra;
  /// Starting symbol for the first PRACH TD occasion.
  /// \remark See TS38.211, sec 6.3.3.2 and Tables 6.3.3.2-2 and 6.3.3.2-4. Possible values: {0,...,13}.
  uint8_t start_symbol;
  /// N-CS configuration as per TS38.211, Table 6.3.3.1-5. Possible values: {0,...,419}.
  uint16_t nof_cs;
  /// Number of frequency domain occasions starting with index_fd_ra. Possible values: {1,...,8}.
  uint8_t nof_fd_ra;
  /// Start of preamble logical index to monitor the PRACH occasions in this slot. Values: {0,...63}.
  uint8_t start_preamble_index;
  /// Number of preamble logical indices. Values: {1,...,64}.
  uint8_t nof_preamble_indexes;
};

/// Info about PUCCH used resource.
struct pucch_info {
  rnti_t                   crnti;
  const bwp_configuration* bwp_cfg;
  pucch_format             format;
  /// PRBs and symbols for this PUCCH resource.
  pucch_resources resources;
  union {
    pucch_format_0 format_0;
    pucch_format_1 format_1;
    pucch_format_2 format_2;
    pucch_format_3 format_3;
    pucch_format_4 format_4;
  };
  /// In case the PUCCH will contain CSI bits, this struct contains information how those bits are to be decoded.
  optional<csi_report_configuration> csi_rep_cfg;
};

struct ul_sched_result {
  /// Number of UL symbols active for this slot.
  unsigned nof_ul_symbols;
  /// PUSCH grants allocated in the current slot.
  static_vector<ul_sched_info, MAX_PUSCH_PDUS_PER_SLOT> puschs;
  /// PRACH occasions within the given slot.
  static_vector<prach_occasion_info, MAX_PRACH_OCCASIONS_PER_SLOT> prachs;
  /// PUCCH grants allocated in the current slot.
  static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT> pucchs;
};

/// Scheduler decision made for DL and UL in a given slot.
struct sched_result {
  bool            success;
  dl_sched_result dl;
  ul_sched_result ul;
};

class scheduler_slot_handler
{
public:
  /// \brief Effect that the errors in the lower layers had on the results provided by the scheduler for a given
  /// slot and cell.
  struct error_outcome {
    bool pdcch_discarded : 1;
    bool pdsch_discarded : 1;
    bool pusch_and_pucch_discarded : 1;
  };

  virtual ~scheduler_slot_handler() = default;

  /// \brief Handle slot indications that arrive to the scheduler for a given cell.
  virtual const sched_result& slot_indication(slot_point sl_tx, du_cell_index_t cell_index) = 0;

  /// \brief Handle error indications caused by lates or invalid scheduling results.
  virtual void handle_error_indication(slot_point sl_tx, du_cell_index_t cell_index, error_outcome event) = 0;
};

} // namespace srsran
