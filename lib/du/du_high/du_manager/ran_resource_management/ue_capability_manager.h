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

#include "du_drx_resource_manager.h"
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
  explicit ue_capability_manager(span<const du_cell_config> cell_cfg_list,
                                 du_drx_resource_manager&   drx_mng_,
                                 srslog::basic_logger&      logger_);

  /// \brief Called on creation of a new UE.
  ///
  /// This method is used to set the right default values for several of the UE resources that depend on the UE
  /// capabilities.
  void handle_ue_creation(du_ue_resource_config& ue_res_cfg);

  /// \brief Called on each UE capability RAT list container received from the CU-CP, to update the UE DU-specific
  /// resources.
  /// \param[in/out] ue_res_cfg current UE resource configuration, which will be updated inplace, depending on the UE
  /// capabilities received.
  /// \param[in] ue_cap_rat_list packed UE capability RAT list container.
  void update(du_ue_resource_config& ue_res_cfg, const byte_buffer& ue_cap_rat_list);

  /// \brief Called on UE removal, to release its resources.
  void release(du_ue_resource_config& ue_res_cfg);

private:
  // Helper function to decode a packed UE capability RAT list. Returns true, if the packed container was successfully
  // decoded and processed.
  bool decode_ue_capability_list(const byte_buffer& ue_cap_rat_list);

  pdsch_mcs_table select_pdsch_mcs_table(du_cell_index_t cell_idx) const;
  pusch_mcs_table select_pusch_mcs_table(du_cell_index_t cell_idx) const;

  /// Selects the PUSCH transmission codebook subset.
  tx_scheme_codebook_subset select_tx_codebook_subset(du_cell_index_t cell_idx) const;
  /// Selects the SRS transmission number of ports.
  unsigned select_srs_nof_ports(du_cell_index_t cell_idx) const;
  /// Selects the PUSCH maximum number of layers.
  unsigned select_pusch_max_rank(du_cell_index_t cell_idx) const;
  // Updates the DRX config of the UE.
  void update_drx(du_ue_resource_config& ue_res_cfg);

  span<const du_cell_config> base_cell_cfg_list;
  // Allocator of DRX and measGap resources for the cellGroup.
  du_drx_resource_manager& drx_res_mng;
  srslog::basic_logger&    logger;

  // Flag that tells whether this is the first time that update is called.
  std::optional<ue_capability_summary> ue_caps;
};

} // namespace srs_du
} // namespace srsran
