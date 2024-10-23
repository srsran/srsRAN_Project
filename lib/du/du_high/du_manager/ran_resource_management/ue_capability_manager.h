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

#include "du_ue_resource_config.h"

namespace asn1 {
namespace rrc_nr {

struct ue_nr_cap_s;

}
} // namespace asn1

namespace srsran {

struct scheduler_expert_config;

namespace srs_du {

/// Flat structure summarizing the decoded ASN.1 UE capabilities.
struct ue_capability_summary {
  /// Contains band specific parameters.
  struct supported_band {
    /// Set to true if QAM-256 is supported for PUSCH transmissions.
    bool pusch_qam256_supported = false;
    /// \brief PUSCH transmit coherence.
    ///
    /// It is given by field \e pusch-TransCoherence in Information Element \e MIMO-ParametersPerBand.
    ///
    /// The most limiting transmit codebook subset is selected by default.
    tx_scheme_codebook_subset pusch_tx_coherence = tx_scheme_codebook_subset::non_coherent;
  };

  /// Set to true if QAM-256 MCS table are supported for PDSCH transmissions.
  bool pdsch_qam256_supported = false;
  /// Number of Sounding Reference Signals transmit ports. It is set to the most limiting band combination of
  /// transmitters.
  uint8_t nof_srs_tx_ports = 1;
  /// Contains specific bands capabilities.
  std::unordered_map<nr_band, supported_band> bands;
  /// Set to true if Long DRX cycle is supported.
  bool long_drx_cycle_supported = false;
  /// Set to true if Short DRX cycle is supported.
  bool short_drx_cycle_supported = false;
};

/// Helper function to extract a summary of the UE capabilities based on a packed ASN.1 container.
expected<ue_capability_summary, std::string> decode_ue_nr_cap_container(const byte_buffer& ue_cap_container);

/// Helper function to convert advanced UE NR capabilities.
void decode_advanced_ue_nr_caps(ue_capability_summary& ue_capability, const asn1::rrc_nr::ue_nr_cap_s& ue_cap);

/// Entity responsible for handling the UE RAT capabilities container, passed by the CU-CP, and updating the UE
/// configuration in the DU accordingly.
class ue_capability_manager
{
public:
  explicit ue_capability_manager(span<const du_cell_config> cell_cfg_list, srslog::basic_logger& logger_);

  /// \brief Called on each UE capability RAT list container received from the CU-CP, to update the UE DU-specific
  /// resources.
  /// \param[in/out] ue_res_cfg current UE resource configuration, which will be updated inplace, depending on the UE
  /// capabilities received.
  /// \param[in] ue_cap_rat_list packed UE capability RAT list container.
  void update(du_ue_resource_config& ue_res_cfg, const byte_buffer& ue_cap_rat_list);

private:
  // Helper function to decode a packed UE capability RAT list. Returns true, if the packed container was successfully
  // decoded and processed.
  bool decode_ue_capability_list(const byte_buffer& ue_cap_rat_list);

  pdsch_mcs_table select_pdsch_mcs_table(du_cell_index_t cell_idx) const;
  pusch_mcs_table select_pusch_mcs_table(du_cell_index_t cell_idx) const;

  /// Selects the PUSCH transmission codebook subset.
  tx_scheme_codebook_subset select_tx_codebook_subset(du_cell_index_t cell_idx) const;
  /// Selects the SRS transmission number of ports.
  unsigned select_srs_nof_ports() const;

  span<const du_cell_config> base_cell_cfg_list;
  srslog::basic_logger&      logger;

  // Flag that tells whether this is the first time that update is called.
  bool                                 first_update = true;
  std::optional<ue_capability_summary> ue_caps;
};

} // namespace srs_du
} // namespace srsran
