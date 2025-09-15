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

namespace srsran {
namespace srs_du {

/// Flat structure summarizing the decoded ASN.1 UE capabilities.
struct ue_capability_summary {
  /// \defgroup default_caps Default parameters.
  /// @{
  /// Default PUSCH transmit coherence.
  static constexpr tx_scheme_codebook_subset default_pusch_tx_coherence = tx_scheme_codebook_subset::non_coherent;
  /// Default PUSCH maximum number of layers.
  static constexpr unsigned default_pusch_max_rank = 1;
  /// Default SRS number of transmit ports.
  static constexpr unsigned default_nof_srs_tx_ports = 1;
  /// Default max number of DL/UL HARQ processes.
  static constexpr unsigned default_max_harq_process_num = 16;
  /// @}

  /// Contains band specific parameters.
  struct supported_band {
    /// Set to true if QAM-256 is supported for PUSCH transmissions.
    bool pusch_qam256_supported = false;
    /// \brief PUSCH transmit coherence.
    ///
    /// It is given by field \e pusch-TransCoherence in Information Element \e MIMO-ParametersPerBand.
    ///
    /// The most limiting transmit codebook subset is selected by default.
    tx_scheme_codebook_subset pusch_tx_coherence = default_pusch_tx_coherence;
    /// Maximum PUSCH number of layers.
    unsigned pusch_max_rank = default_pusch_max_rank;
    /// Maximum number of ports that can be simultaneously used for transmiting Sounding Reference Signals.
    uint8_t nof_srs_tx_ports = default_nof_srs_tx_ports;
    /// Maximum number of DL HARQ processes.
    uint8_t max_dl_harq_process_num = default_max_harq_process_num;
    /// Maximum number of UL HARQ processes.
    uint8_t max_ul_harq_process_num = default_max_harq_process_num;
    /// Indicates whether the UE supports the uplink time and frequency pre-compensation.
    bool ul_pre_compensation_supported = false;
    /// Indicates whether the UE supports UE reporting of information related to TA pre-compensation.
    bool ul_ta_reporting_supported = false;
    /// Indicates whether the UE supports the reception of UE-specific K_offset.
    bool ue_specific_k_offset_supported = false;
  };

  /// Set to true if QAM-256 MCS table are supported for PDSCH transmissions.
  bool pdsch_qam256_supported = false;
  /// Set to true if QAM-64 LowSe MCS table are supported for PDSCH transmissions.
  bool pdsch_qam64lowse_supported = false;
  /// Set to true if QAM-64 LowSe MCS table are supported for PUSCH transmissions.
  bool pusch_qam64lowse_supported = false;
  /// Contains specific bands capabilities.
  std::unordered_map<nr_band, supported_band> bands;
  /// Set to true if Long DRX cycle is supported.
  bool long_drx_cycle_supported = false;
  /// Set to true if Short DRX cycle is supported.
  bool short_drx_cycle_supported = false;
  /// Set to true if UE supports \c interleavingVRB-ToPRB-PDSCH, as per TS 38.306, Section 4.2.7.10.
  bool pdsch_interleaving_vrb_to_prb_supported = false;
  /// Set to true if UE supports NTN features.
  bool ntn_supported = false;
  /// Indicates whether the UE supports disabled HARQ feedback for downlink transmission.
  bool disabled_dl_harq_feedback_supported = false;
  /// Indicates whether the UE supports HARQ Mode B and the corresponding LCP restrictions for uplink transmission.
  bool ul_harq_mode_b_supported = false;
};

} // namespace srs_du
} // namespace srsran
