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
