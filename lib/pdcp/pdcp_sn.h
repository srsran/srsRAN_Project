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

#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

constexpr uint32_t pdcp_compute_sn(uint32_t count, pdcp_sn_size sn_size)
{
  return count & (0xffffffffU >> (32U - pdcp_sn_size_to_uint(sn_size)));
}

constexpr uint32_t pdcp_compute_hfn(uint32_t count, pdcp_sn_size sn_size)
{
  return (count >> pdcp_sn_size_to_uint(sn_size));
}

constexpr uint32_t pdcp_compute_count(uint32_t hfn, uint32_t sn, pdcp_sn_size sn_size)
{
  return (hfn << pdcp_sn_size_to_uint(sn_size)) | sn;
}

constexpr uint32_t pdcp_sn_cardinality(pdcp_sn_size sn_size)
{
  uint16_t sn_size_num = pdcp_sn_size_to_uint(sn_size);
  srsran_assert(sn_size_num < 32, "Cardinality of sn_size={} exceeds return type 'uint32_t'", sn_size_num);
  return (1 << sn_size_num);
}

} // namespace srsran
