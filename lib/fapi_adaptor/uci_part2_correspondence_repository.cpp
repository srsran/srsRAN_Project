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
