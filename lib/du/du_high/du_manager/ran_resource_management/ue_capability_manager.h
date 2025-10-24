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
#include "ue_capability_summary.h"

namespace asn1 {
namespace rrc_nr {

struct ue_nr_cap_s;

}
} // namespace asn1

namespace srsran {

struct scheduler_expert_config;

namespace srs_du {

/// Helper function to extract a summary of the UE capabilities based on a packed ASN.1 container.
expected<ue_capability_summary, std::string> decode_ue_nr_cap_container(const byte_buffer& ue_cap_container);

/// Helper function to convert advanced UE NR capabilities.
void decode_advanced_ue_nr_caps(ue_capability_summary& ue_capability, const asn1::rrc_nr::ue_nr_cap_s& ue_cap);

/// Helper function to convert advanced UE NR capabilities.
void decode_advanced_ue_nr_ntn_caps(ue_capability_summary& ue_capability, const asn1::rrc_nr::ue_nr_cap_s& ue_cap);

/// Entity responsible for handling the UE RAT capabilities container, passed by the CU-CP, and updating the UE
/// configuration in the DU accordingly.
class ue_capability_manager
{
public:
  explicit ue_capability_manager(span<const du_cell_config> cell_cfg_list,
                                 du_drx_resource_manager&   drx_mng_,
                                 srslog::basic_logger&      logger_,
                                 const du_test_mode_config& test_mode_);

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

  /// \brief Stores a previously decoded UE capability RAT list container and updates the UE DU-specific resources.
  /// \param[in/out] ue_res_cfg current UE resource configuration, which will be updated inplace, depending on the UE
  /// capabilities received.
  /// \param[in] summary unpacked UE capability RAT list container.
  void update(du_ue_resource_config& ue_res_cfg, const ue_capability_summary& summary);

  /// \brief Called on UE removal, to release its resources.
  void release(du_ue_resource_config& ue_res_cfg);

  /// Retrieve a summary of the UE capabilities.
  const std::optional<ue_capability_summary>& summary() const { return ue_caps; }

private:
  // Helper function to decode a packed UE capability RAT list. Returns true, if the packed container was successfully
  // decoded and processed.
  bool decode_ue_capability_list(const byte_buffer& ue_cap_rat_list);

  void update_impl(du_ue_resource_config& ue_res_cfg);

  pdsch_mcs_table select_pdsch_mcs_table(du_cell_index_t cell_idx) const;
  pusch_mcs_table select_pusch_mcs_table(du_cell_index_t cell_idx) const;

  vrb_to_prb::mapping_type select_pdsch_interleaving(du_cell_index_t cell_idx) const;

  /// Selects the PUSCH transmission codebook subset.
  tx_scheme_codebook_subset select_tx_codebook_subset(du_cell_index_t cell_idx) const;
  /// Selects the SRS transmission number of ports.
  unsigned select_srs_nof_ports(du_cell_index_t cell_idx) const;
  /// Selects the PUSCH maximum number of layers.
  unsigned select_pusch_max_rank(du_cell_index_t cell_idx) const;
  /// Selects the maximum number of DL HARQ processes.
  unsigned select_max_dl_nof_harqs(du_cell_index_t cell_idx) const;
  /// Selects the maximum number of DL HARQ processes.
  unsigned select_max_ul_nof_harqs(du_cell_index_t cell_idx) const;
  /// Selects the DL HARQ Process Number field size.
  unsigned select_dl_dci_harq_num_field_size(du_cell_index_t cell_idx) const;
  /// Selects the UL HARQ Process Number field size.
  unsigned select_ul_dci_harq_num_field_size(du_cell_index_t cell_idx) const;
  /// Selects the UL HARQ Mode B.
  bool select_ul_harq_mode_b(du_cell_index_t cell_idx) const;

  // Updates the DRX config of the UE.
  void update_drx(du_ue_resource_config& ue_res_cfg);

  span<const du_cell_config> base_cell_cfg_list;
  // Allocator of DRX and measGap resources for the cellGroup.
  du_drx_resource_manager&   drx_res_mng;
  srslog::basic_logger&      logger;
  const du_test_mode_config& test_cfg;

  // Flag that tells whether this is the first time that update is called.
  std::optional<ue_capability_summary> ue_caps;
};

} // namespace srs_du
} // namespace srsran
