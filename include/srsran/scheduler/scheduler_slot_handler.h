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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_rs/csi_rs_types.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/logical_channel/lcid_dl_sch.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/ran/uci/uci_configuration.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/result/pdcch_info.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/scheduler/scheduler_pucch_format.h"
#include "srsran/scheduler/vrb_alloc.h"

namespace srsran {

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
    std::optional<uint8_t> beta_offset_csi_2;
  };

  std::optional<harq_info> harq;
  std::optional<csi_info>  csi;
  /// \f$\alpha\f$ parameter, as per Section 6.3.2.4.1.1-3, TS 38.212.
  alpha_scaling_opt alpha;
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
  pdcch_dl_info_list dl_pdcchs;

  /// Allocated UL PDCCHs.
  pdcch_ul_info_list ul_pdcchs;

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
  pusch_information       pusch_cfg;
  std::optional<uci_info> uci;

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
    std::optional<unsigned> msg3_delay;
    /// Offset that the OLLA algorithm applied to derive the UL MCS.
    std::optional<float> olla_offset;
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
  /// This information only is used by the scheduler and not passed to the PHY.
  struct context {
    /// Identifier of the PUCCH PDU within the list of PUCCH PDUs for a given slot. The ID is only meaningful for a
    /// given UE; i.e., different UEs can reuse the same ID, but a UE cannot reuse the same ID for different PDUs.
    unsigned id = MAX_PUCCH_PDUS_PER_SLOT;
    /// Determines whether the PUCCH PDU uses common resources.
    bool is_common = false;
  };

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
  std::optional<csi_report_configuration> csi_rep_cfg;

  context pdu_context;
};

struct srs_info {
  rnti_t                   crnti;
  const bwp_configuration* bwp_cfg;
  uint8_t                  nof_antenna_ports;
  /// Symbols used for this SRS resource, starting from \f$l_0\f$ until \f$l_0 + n^{SRS}_{symb}\f$, as per
  /// Section 6.4.1.4.1, TS 38.211.
  ofdm_symbol_range symbols;
  /// Repetition factor \f$R\f$, or \c repetitionFactor, as per \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// \remark As per TS 38.211, Section 6.4.1.4.3, the number of repetitions must be not greater than the \ref symbols
  /// length.
  srs_nof_symbols nof_repetitions;
  /// Configuration index, given by \c c_SRS, as per \c freqHopping, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,63}.
  uint8_t config_index;
  /// SRS sequence ID or \c sequenceId, as per \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,1023}.
  unsigned sequence_id;
  /// Bandwidth index, given by \c b_SRS, as per \c freqHopping, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,3}.
  uint8_t bw_index;
  /// Transmission comb size, as per \c transmissionComb, in \c SRS-Config, TS 38.331, or \f$K_{TC}\f$, as per
  /// Section 6.4.1.4.1, TS 38.211.
  tx_comb_size tx_comb;
  /// Transmission comb offset, given in \c combOffset-n2 or \c combOffset-n4, \c transmissionComb, \c SRS-Resource,
  /// in \c SRS-Config, TS 38.331. Values {0, 1} if tx_comb_size == 2, {0,...,3} if tx_comb_size == 4.
  uint8_t comb_offset;
  /// Cyclic shift, given in \c cyclicShift-n2 or \c cyclicShift-n4, \c transmissionComb, \c SRS-Resource, in \c
  /// SRS-Config, TS 38.331. Values {0,...,7} if tx_comb_size == 2, {0,...,11} if tx_comb_size == 4.
  uint8_t cyclic_shift;
  /// Frequency domain position \c freqDomainPosition, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,67}.
  uint8_t freq_position;
  /// Frequency domain shift \c freqDomainShift, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,268}.
  unsigned freq_shift;
  /// Frequency hopping \c b_hop, as per \c freqHopping, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,3}.
  uint8_t                       freq_hopping;
  srs_group_or_sequence_hopping group_or_seq_hopping;
  srs_resource_type             resource_type;
  /// SRS periodicity in slots, as per \c SRS-PeriodicityAndOffset, in \c SRS-Config, TS 38.331.
  /// \remark Only applies if resource_type == periodic or resource_type == semi_persistent.
  srs_periodicity t_srs_period;
  /// SRS offset in slots, as per \c SRS-PeriodicityAndOffset, in \c SRS-Config, TS 38.331.
  /// Values { 0,...,t_srs_period - 1}.
  /// \remark Only applies if resource_type == periodic or resource_type == semi_persistent.
  unsigned t_offset;
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
  /// SRS grants allocated in the current slot.
  static_vector<srs_info, MAX_SRS_PDUS_PER_SLOT> srss;
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
