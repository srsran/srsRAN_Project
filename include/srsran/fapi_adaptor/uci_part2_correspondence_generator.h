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

#include "srsran/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_repository.h"

namespace srsran {
namespace fapi_adaptor {

/// Generates the UCI Part2 correspondence mapper and repository for the given number of CSI-RS resources.
std::pair<std::unique_ptr<uci_part2_correspondence_mapper>, std::unique_ptr<uci_part2_correspondence_repository>>
generate_uci_part2_correspondence(unsigned nof_csi_rs_resources);

} // namespace fapi_adaptor
} // namespace srsran
