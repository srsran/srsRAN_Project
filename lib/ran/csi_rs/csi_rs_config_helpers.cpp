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

#include "srsran/ran/csi_rs/csi_rs_config_helpers.h"

using namespace srsran;
using namespace csi_rs;

// CSI-RS locations within a slot. See TS 38.211 Table 7.4.1.5.3-1.
namespace {
struct csi_rs_resource_mapping_info {
  unsigned                 row;
  uint8_t                  nof_ports;
  csi_rs_freq_density_type density;
  csi_rs_cdm_type          cdm_type;
};
} // namespace

// The following entries are taken from Table 7.4.1.5.3-1, TS 38.211, which is spread out over all possible combinations
// of nof_ports, density, cdm_type.
static const uint32_t nof_csi_rs_resource_mappings = 38;
static constexpr std::array<csi_rs_resource_mapping_info, nof_csi_rs_resource_mappings>
    csi_rs_resource_mapping_within_slot = {{
        // clang-format off
    {1,  1, csi_rs_freq_density_type::three, csi_rs_cdm_type::no_CDM},
    {2,  1, csi_rs_freq_density_type::one, csi_rs_cdm_type::no_CDM},
    {2,  1, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::no_CDM},
    {2,  1, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::no_CDM},
    {3,  2, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {3,  2, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {3,  2, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {4,  4, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {5,  4, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {6,  8, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {7,  8, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {8,  8, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {9,  12, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {10,  12, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {11,  16, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {11,  16, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {11,  16, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {12,  16, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {12,  16, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {12,  16, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {13,  24, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {13,  24, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {13,  24, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {14,  24, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {14,  24, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {14,  24, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {15,  24, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm8_FD2_TD4},
    {15,  24, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm8_FD2_TD4},
    {15,  24, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm8_FD2_TD4},
    {16,  32, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {16,  32, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {16,  32, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {17,  32, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {17,  32, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {17,  32, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {18,  32, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm8_FD2_TD4},
    {18,  32, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm8_FD2_TD4},
    {18,  32, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm8_FD2_TD4}
        // clang-format on
    }};

unsigned srsran::csi_rs::get_csi_rs_resource_mapping_row_number(uint8_t                          nof_ports,
                                                                csi_rs_freq_density_type         density,
                                                                csi_rs_cdm_type                  cdm_type,
                                                                const freq_allocation_mask_type& fd_alloc)
{
  // The following mapping can be inferred from Section 7.4.1.5.3, TS 38.211.
  for (const csi_rs_resource_mapping_info& info : csi_rs_resource_mapping_within_slot) {
    if (info.nof_ports == nof_ports and info.cdm_type == cdm_type and info.density == density) {
      // As per Table 7.4.1.5.3-1, TS 38.211, rows 6 and 7 have the same nof_ports, cdm_type and density. The only way
      // we can tell them apart is by checking the number of bits set to 1 in the frequency allocation vector \c
      // frequencyDomainAllocation, which is 2 and 6 bits for row 6 and 7, respectively, as per Section 7.4.1.5.3,
      // TS 38.211.
      if (info.row == 6U or info.row == 7U) {
        if (fd_alloc.count() == 2U) {
          return 7U;
        } else if (fd_alloc.count() == 4U) {
          return 6U;
        }
        continue;
      }
      // As per Table 7.4.1.5.3-1, TS 38.211, rows 4 and 5 have the same nof_ports, cdm_type and density. The only way
      // we can tell them apart is by checking the size of the frequency allocation vector \c frequencyDomainAllocation,
      // which is 3 and 6 bits for row 4 and row 5 respectively.
      if (info.row == 4U or info.row == 5U) {
        if (fd_alloc.size() == 3) {
          return 4U;
        } else if (fd_alloc.size() == 6) {
          return 5U;
        }
        continue;
      }
      return info.row;
    }
  }
  srsran_assertion_failure("The CSI-RS resource config does not match any row of Table 7.4.1.5.3-1, TS 38.211.");
  const unsigned invalid_row_index = 0;
  return invalid_row_index;
}

unsigned srsran::csi_rs::get_nof_csi_rs_ports(unsigned csi_rs_row_id)
{
  switch (csi_rs_row_id) {
    case 1:
    case 2:
      return 1;
    case 3:
      return 2;
    case 4:
    case 5:
      return 4;
    case 6:
    case 7:
    case 8:
      return 8;
    case 9:
    case 10:
      return 12;
    case 11:
    case 12:
      return 16;
    case 13:
    case 14:
    case 15:
      return 24;
    case 16:
    case 17:
    case 18:
      return 32;
    default:
      srsran_assertion_failure("Invalid CSI-RS mapping table row, i.e., {}.", csi_rs_row_id);
      return 0;
  }
}
