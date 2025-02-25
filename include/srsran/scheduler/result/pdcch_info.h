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

#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/scheduler/result/dci_info.h"

namespace srsran {

struct bwp_configuration;
struct coreset_configuration;

/// The precoding information associated with PDCCH PDUs.
struct pdcch_precoding_info {};

/// Transmit power information associated with PDCCH PDU.
struct tx_power_pdcch_information {
  /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE. See 3GPP TS 38.214, clause 5.2.2.3.1. Values {-3, 0, 3, 6} dB.
  /// \remark If the UE has not been provided dedicated higher layer parameters, the UE may assume that the ratio of
  /// PDCCH DMRS EPRE to SSS EPRE is within -8 dB and 8 dB when the UE monitors PDCCHs for a DCI format 1_0 with CRC
  /// scrambled by SI-RNTI, P-RNTI, or RA-RNTI. See TS 38.213, clause 4.1.
  /// \remark [Implementation-defined] In case UE is not configured with powerControlOffsetSS we assume it to be 0dB.
  int8_t pwr_ctrl_offset_ss{0};
};

/// Information relative to the allocation of PDCCH for DCI.
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
  std::optional<pdcch_precoding_info> precoding_info;
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
    std::optional<unsigned> harq_feedback_timing;
  } context;
};

/// PDCCH DL allocation.
struct pdcch_dl_information {
  /// Context associated with PDCCH allocation.
  dci_context_information ctx;
  /// DL DCI unpacked content.
  dci_dl_info dci;
};

/// List of DL PDCCH allocations within a given slot.
using pdcch_dl_info_list = static_vector<pdcch_dl_information, MAX_DL_PDCCH_PDUS_PER_SLOT>;

/// PDCCH UL allocation.
struct pdcch_ul_information {
  /// Context associated with PDCCH allocation.
  dci_context_information ctx;
  /// UL DCI unpacked content.
  dci_ul_info dci;
};

/// List of UL PDCCH allocations within a given slot.
using pdcch_ul_info_list = static_vector<pdcch_ul_information, MAX_UL_PDCCH_PDUS_PER_SLOT>;

} // namespace srsran
