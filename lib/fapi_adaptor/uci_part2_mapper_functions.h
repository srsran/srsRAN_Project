/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

namespace srsran {
namespace fapi_adaptor {

constexpr unsigned MAX_NUM_RI_RESTRICTIONS = (1U << 4);
constexpr unsigned MAX_NUM_CSI_RESOURCES   = 1;
constexpr unsigned MAX_NUM_CODEBOOKS       = static_cast<unsigned>(pmi_codebook_type::other);
constexpr unsigned MAX_NUM_QUANTITIES      = static_cast<unsigned>(csi_report_quantities::other);

/// Returns the UCI Part2 correspondence index using the given parameters.
inline unsigned get_uci_part2_correspondence_index(unsigned nof_csi_resources,
                                                   unsigned pmi_codebook,
                                                   unsigned ri_restriction,
                                                   unsigned quantities)
{
  return (MAX_NUM_RI_RESTRICTIONS * MAX_NUM_CSI_RESOURCES * MAX_NUM_CODEBOOKS * quantities) +
         (MAX_NUM_CSI_RESOURCES * MAX_NUM_CODEBOOKS * ri_restriction) + (MAX_NUM_CSI_RESOURCES * pmi_codebook) +
         nof_csi_resources;
}

} // namespace fapi_adaptor
} // namespace srsran
