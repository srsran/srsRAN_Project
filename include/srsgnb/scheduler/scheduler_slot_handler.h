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
#include "prb_grant.h"
#include "sched_consts.h"
#include "scheduler_dci.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/modulation_scheme.h"
#include "srsgnb/ran/ofdm_symbol_range.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/pdsch/pdsch_mcs.h"
#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/pusch/pusch_mcs.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/scheduler/dmrs.h"
#include <cstddef>

namespace srsgnb {

/// Maximum grants per slot. Implementation-specific.
const size_t MAX_GRANTS = 16;
/// Maximum Logical channels per TB. Implementation-specific.
const size_t MAX_LC_GRANTS = 4;
/// Maximum SSB opportunity per slot. This can be derived from the candidate ODFM symbols indices within the ranges
/// 0-13, 14-27, 28-41, 42-55, etc.. from TS 38.213, Section 4.1.
const size_t MAX_SSB_PER_SLOT = 2;
/// [Implementation defined] This corresponds to "Number of search space sets per slot" in Table 13-11, TS 38.213.
constexpr size_t MAX_SIB1_PDUS_PER_SLOT = 2;
/// [Implementation defined] This corresponds to maximum number of RARs that can be scheduled per slot.
constexpr size_t MAX_RAR_PDUS_PER_SLOT = 16;
/// Maximum number of PRACH occasions within a slot as per TS38.211, Tables 6.3.3.2-[2-4] and maximum msg1-FDM of 8
/// according to TS 38.331.
const static size_t MAX_NOF_PRACHS_PER_SLOT = 56;
/// Maximum number of codewords per PDSCH grant. Implementation-specific.
const static size_t MAX_NOF_CODEWORDS_PER_PDSCH = 2;

struct beamforming_info {
  // TODO
};

struct tx_power_pdcch_information {
  // TODO
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
  /// \brief DMRS sequence initialization (see TS 38.221 sec 7.4.1.1.2), as provided by parameter n_{SCID}.
  /// Values: false -> 0, true -> 1.
  bool n_scid;
  /// Values: (1..3).
  uint8_t num_dmrs_cdm_grps_no_data;
  /// \brief Bitmap of antenna ports. Bit 0 corresponds to antenna port 1000 and bit 11 to antenna port 1011, and
  /// each bit=1 mean DM-RS port used.
  bounded_bitset<12> dmrs_ports;
};

struct dci_dl_context_information {
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
  /// Precoding and beamforming info used for this DCI.
  beamforming_info bf;
  /// Transmission power information used for this DCI.
  tx_power_pdcch_information tx_pwr;
};

/// PDCCH DL allocation.
struct pdcch_dl_information {
  /// Context associated with PDCCH allocation.
  dci_dl_context_information ctx;
  /// DL DCI unpacked content.
  dci_dl_info dci;
};

/// PDCCH UL allocation.
struct pdcch_ul_information {
  /// Context associated with PDCCH allocation.
  dci_dl_context_information ctx;
  /// UL DCI unpacked content.
  dci_ul_info dci;
};

/// PDSCH codeword.
/// \remark See FAPI PDSCH PDU.
struct pdsch_codeword {
  /// Target code rate, normalized to 1024 (see TS38.214 Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3).
  float target_code_rate;
  /// Subcarrier modulation scheme, as per TS38.211 Section 5.1.
  modulation_scheme qam_mod;
  /// \brief MCS index, range {0, ..., 31} (See TS38.214 Section 5.1.3.1).
  /// \note Should match value sent in DCI.
  sch_mcs_index mcs_index;
  /// MCS table (See TS38.214 Section 5.1.3.1).
  pdsch_mcs_table mcs_table;
  /// Redundancy version index (see TS38.212 Table 5.4.2.1-2, and TS38.214 Table 5.1.2.1-2).
  uint8_t rv_index;
  /// Transport block size, in bytes (see TS38.214 Section 5.1.3.2).
  uint32_t tb_size_bytes;
};

/// Information related to a PDSCH allocation.
/// \remark See FAPI PDSCH PDU.
struct pdsch_information {
  rnti_t                                                     rnti;
  const bwp_configuration*                                   bwp_cfg;
  prb_grant                                                  prbs;
  ofdm_symbol_range                                          symbols;
  static_vector<pdsch_codeword, MAX_NOF_CODEWORDS_PER_PDSCH> codewords;
  dmrs_information                                           dmrs;
  /// Parameter n_ID, used for scrambling, as per TS 38.211, Section 7.3.1.1.
  unsigned n_id;
};

struct dl_msg_lc_info {
  /// LCID {0..32}.
  lcid_t lcid;
  /// Number of scheduled bytes for this specific logical channel. {0..65535}.
  unsigned sched_bytes;
};

struct dl_msg_tb_info {
  /// List of allocated logical channels.
  static_vector<dl_msg_lc_info, MAX_LC_GRANTS> lc_lst;
};

/// Dedicated DL Grant for UEs.
struct dl_msg_alloc {
  pdsch_information                             pdsch_cfg;
  static_vector<dl_msg_tb_info, MAX_NOF_LAYERS> tb_list;
};

struct pusch_information {
  rnti_t                   rnti;
  const bwp_configuration* bwp_cfg;
  prb_grant                prbs;
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
  /// Target code rate, normalized to 1024 (see TS38.214 Table 6.1.4.1).
  float target_code_rate;
  /// Subcarrier modulation scheme, as per TS38.211 Section 6.1.
  modulation_scheme qam_mod;
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
  /// \brief PUSCH DMRS hopping mode as per TS38.211, Section 6.4.1.1.1.2. It is only valid when the transform
  /// precoding for PUSCH is enabled.
  enum class dmrs_hopping_mode { no_hopping, group_hopping, sequence_hopping } dmrs_hopping_mode;
  /// Redundancy version index (see TS38.214 Table 6.1.4). Values: {0,...,3}.
  uint8_t rv_index;
  /// HARQ process number as per TS38.212 Section 7.3.1.1. Values: {0,...,15}.
  uint8_t harq_id;
  /// \brief Signals whether the PUSCH PDU corresponds to an initial transmission or a retransmission of a MAC PDU for
  /// this HARQ process ID for this TB.
  bool new_data;
  /// Transport block size in bytes.
  uint32_t tb_size_bytes;
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
  const pdcch_dl_information*             pdcch_cfg;
  pdsch_information                       pdsch_cfg;
  static_vector<rar_ul_grant, MAX_GRANTS> grants;
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
  unsigned                    nof_txs;
  const pdcch_dl_information* pdcch_cfg;
  pdsch_information           pdsch_cfg;
};

/// See ORAN WG8, 9.2.3.3.12 - Downlink Broadcast Allocation.
struct dl_broadcast_allocation {
  static_vector<ssb_information, MAX_SSB_PER_SLOT>       ssb_info;
  static_vector<sib_information, MAX_SIB1_PDUS_PER_SLOT> sibs;
};

struct dl_sched_result {
  /// Allocated DL PDCCHs. Includes both SIB, RAR and Data PDCCHs.
  static_vector<pdcch_dl_information, MAX_GRANTS> dl_pdcchs;

  /// Allocated UL PDCCHs.
  static_vector<pdcch_ul_information, MAX_GRANTS> ul_pdcchs;

  /// Allocation of SSB and SIBs.
  dl_broadcast_allocation bc;

  /// Allocation of dedicated RARs.
  static_vector<rar_information, MAX_GRANTS> rar_grants;

  /// Allocation of dedicated UE messages.
  static_vector<dl_msg_alloc, MAX_GRANTS> ue_grants;
};

struct ul_sched_info {
  pusch_information pusch_cfg;
};

struct prach_occasion_info {
  /// Physical Cell identifier.
  pci_t pci;
  /// Number of time-domain PRACH occasions (\f$N^{RAslot}_t\f$), as per TS38.211 Tables 6.3.3.2-[2-4].
  uint8_t nof_prach_occasions;
  /// RACH format information for the PRACH occasions.
  preamble_format format;
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

struct ul_sched_result {
  /// PUSCH grants allocated in the current slot.
  static_vector<ul_sched_info, MAX_GRANTS> puschs;
  /// PRACH occasions within the given slot.
  static_vector<prach_occasion_info, MAX_NOF_PRACHS_PER_SLOT> prachs;
};

/// Scheduler decision made for DL and UL in a given slot.
struct sched_result {
  dl_sched_result dl;
  ul_sched_result ul;
};

class scheduler_slot_handler
{
public:
  virtual ~scheduler_slot_handler()                                                         = default;
  virtual const sched_result* slot_indication(slot_point sl_tx, du_cell_index_t cell_index) = 0;
};

} // namespace srsgnb
