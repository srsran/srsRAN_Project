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

#include "srsran/adt/expected.h"
#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/compiler.h"
#include <vector>

namespace srsran {

class gtpu_teid_pool_impl final : public gtpu_teid_pool
{
public:
  explicit gtpu_teid_pool_impl(uint32_t max_teids_) : max_teids(max_teids_), teid_pool(max_teids_) {}

  SRSRAN_NODISCARD expected<gtpu_teid_t> request_teid() override
  {
    expected<gtpu_teid_t> teid;
    teid.set_error(default_error_t{});

    if (full()) {
      return teid;
    }

    // Find a free teid
    bool     found = false;
    uint16_t tmp   = next_teid;
    for (uint16_t n = 0; n < max_teids; n++) {
      tmp = (next_teid + n) % max_teids;
      if (not teid_pool[tmp]) {
        teid_pool[tmp] = true;
        found          = true;
        break;
      }
    }

    if (not found) {
      return teid;
    }
    next_teid = (tmp + 1) % max_teids;
    teid      = gtpu_teid_t{tmp};
    nof_teids++;
    return teid;
  }

  SRSRAN_NODISCARD bool release_teid(gtpu_teid_t teid) override
  {
    if (not teid_pool[teid.value()]) {
      // trying to free non-allocated TEID
      return false;
    }
    teid_pool[teid.value()] = false;
    nof_teids--;
    return true;
  }

  bool full() const override { return nof_teids >= max_teids; }

  uint32_t get_max_teids() override { return max_teids; }

private:
  uint32_t       next_teid = 0;
  uint32_t       nof_teids = 0;
  const uint32_t max_teids;

  std::vector<bool> teid_pool;
};
} // namespace srsran
