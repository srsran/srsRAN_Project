/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
};

} // namespace srs_du
} // namespace srsran
