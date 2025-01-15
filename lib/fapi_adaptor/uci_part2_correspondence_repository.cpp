/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi_adaptor/uci_part2_correspondence_repository.h"

using namespace srsran;
using namespace fapi_adaptor;

span<const uint16_t> uci_part2_correspondence_repository::get_uci_part2_correspondence(unsigned index) const
{
  srsran_assert(index < repo_map.size(),
                "Invalid UCI Part2 correspondence index, index value is '{}' while repository size has '{}' entries",
                index,
                repo_map.size());

  return repo_map[index];
}
