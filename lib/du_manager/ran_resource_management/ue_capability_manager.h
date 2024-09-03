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

#include "du_ue_resource_config.h"

namespace srsran {

struct scheduler_expert_config;

namespace srs_du {

/// Flat structure summarizing the decoded ASN.1 UE capabilities.
struct ue_capability_summary {
  struct supported_band {
    nr_band band;
    bool    pusch_qam256_supported = false;
  };

  bool                        pdsch_qam256_supported = false;
  std::vector<supported_band> bands;
};

/// Helper function to extract a summary of the UE capabilities based on a packed ASN.1 container.
expected<ue_capability_summary, std::string> decode_ue_nr_cap_container(const byte_buffer& ue_cap_container);

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

  span<const du_cell_config> base_cell_cfg_list;
  srslog::basic_logger&      logger;

  // flag that tells whether this is the first time that update is called.
  bool                                 first_update = true;
  std::optional<ue_capability_summary> ue_caps;
};

} // namespace srs_du
} // namespace srsran
