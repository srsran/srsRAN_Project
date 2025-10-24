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

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/uci/uci_configuration.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/result/dmrs_info.h"
#include "srsran/scheduler/result/vrb_alloc.h"

namespace srsran {

/// Information relative to a PUSCH allocation.
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
  /// HARQ process number as per TS38.212 Section 6.3.1.1. Values: {0,...,31}.
  harq_id_t harq_id;
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

/// Information related to a scheduler UL allocation, which includes PUSCH and may include UCI.
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
    /// Number of overhead PRBs.
    unsigned nof_oh_prb;
  } context;
};

} // namespace srsran
