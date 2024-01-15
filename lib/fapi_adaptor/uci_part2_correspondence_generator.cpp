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

#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "uci_part2_mapper_functions.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_puxch_utils.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Adds a new entry in the specified position resizing the container if needed.
template <typename T>
static auto add_map_entry(T& container, unsigned index) -> typename T::reference
{
  if (index >= container.size()) {
    container.resize(index + 1U);
  }

  return container[index];
}

std::pair<std::unique_ptr<uci_part2_correspondence_mapper>, std::unique_ptr<uci_part2_correspondence_repository>>
srsran::fapi_adaptor::generate_uci_part2_correspondence(unsigned nof_csi_rs_resources)
{
  srsran_assert(nof_csi_rs_resources && nof_csi_rs_resources <= MAX_NUM_CSI_RESOURCES,
                "Unsupported number of CSI-RS resources");

  unsigned map_index = 0;
  std::vector<static_vector<uci_part2_correspondence_information, uci_part2_size_description::max_nof_entries>>
                                                                                   mapper_map;
  std::vector<static_vector<uint16_t, uci_part2_size_description::max_size_table>> repo_map;

  // Skip generating entries with nof_csi_rs_resources set to 0.
  for (unsigned csi_resource_index = 1; csi_resource_index != (nof_csi_rs_resources + 1); ++csi_resource_index) {
    for (unsigned codebook_index = 0; codebook_index != MAX_NUM_CODEBOOKS; ++codebook_index) {
      // The maximum value of the RI restriction field depends on the number of CSI-RS ports which is derived from the
      // used codebook.
      for (unsigned ri_index = 1,
                    nof_csi_rs_ports =
                        csi_report_get_nof_csi_rs_antenna_ports(static_cast<pmi_codebook_type>(codebook_index)),
                    ri_index_end = pow2(nof_csi_rs_ports);
           ri_index != ri_index_end;
           ++ri_index) {
        for (unsigned quantities_index = 0; quantities_index != MAX_NUM_QUANTITIES; ++quantities_index) {
          ri_restriction_type ri_restriction(nof_csi_rs_ports);
          ri_restriction.from_uint64(ri_index);

          csi_report_configuration report_cfg;
          report_cfg.nof_csi_rs_resources = csi_resource_index;
          report_cfg.pmi_codebook         = static_cast<pmi_codebook_type>(codebook_index);
          report_cfg.ri_restriction       = ri_restriction;
          report_cfg.quantities           = static_cast<csi_report_quantities>(quantities_index);

          uci_part2_size_description part2_correspondence = get_csi_report_pusch_size(report_cfg).part2_correspondence;
          unsigned                   repo_index =
              get_uci_part2_correspondence_index(csi_resource_index, codebook_index, ri_index, quantities_index);
          auto& map_entry = add_map_entry(mapper_map, repo_index);

          for (const auto& entry : part2_correspondence.entries) {
            uci_part2_correspondence_information info;

            // Highest priority.
            info.priority = 0;

            for (const auto& param : entry.parameters) {
              info.part1_param_offsets.push_back(param.offset);
              info.part1_param_sizes.push_back(param.width);
            }

            info.part2_map_index = map_index;

            // PHY specific context.
            info.part2_map_scope = 1;

            map_entry.push_back(info);

            // Store the map of this entry in the repository.
            add_map_entry(repo_map, map_index);
            repo_map[map_index] = entry.map;
            ++map_index;
          }
        }
      }
    }
  }

  return {std::make_unique<uci_part2_correspondence_mapper>(std::move(mapper_map)),
          std::make_unique<uci_part2_correspondence_repository>(std::move(repo_map))};
}
