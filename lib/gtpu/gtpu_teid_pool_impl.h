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

#include "srsran/adt/expected.h"
#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/compiler.h"
#include <vector>

namespace srsran {

class gtpu_teid_pool_impl final : public gtpu_teid_pool
{
public:
  explicit gtpu_teid_pool_impl(uint32_t max_nof_teids_) : max_nof_teids(max_nof_teids_), teid_pool(max_nof_teids_) {}

  SRSRAN_NODISCARD expected<gtpu_teid_t> request_teid() override
  {
    expected<gtpu_teid_t> teid;
    teid.set_error(default_error_t{});

    if (full()) {
      return teid;
    }

    // Find a free teid
    bool     found   = false;
    uint16_t tmp_idx = next_teid_idx;
    for (uint16_t n = 0; n < max_nof_teids; n++) {
      tmp_idx = (next_teid_idx + n) % max_nof_teids;
      if (not teid_pool[tmp_idx]) {
        teid_pool[tmp_idx] = true;
        found              = true;
        break;
      }
    }

    if (not found) {
      return teid;
    }
    next_teid_idx = (tmp_idx + 1) % max_nof_teids;
    teid          = gtpu_teid_t{tmp_idx + GTPU_TEID_MIN.value()};
    nof_teids++;
    return teid;
  }

  SRSRAN_NODISCARD bool release_teid(gtpu_teid_t teid) override
  {
    uint32_t teid_idx = teid.value() - GTPU_TEID_MIN.value();
    if (not teid_pool[teid_idx]) {
      // trying to free non-allocated TEID
      return false;
    }
    teid_pool[teid_idx] = false;
    nof_teids--;
    return true;
  }

  bool full() const override { return nof_teids >= max_nof_teids; }

  uint32_t get_max_nof_teids() override { return max_nof_teids; }

private:
  uint32_t       next_teid_idx = 0;
  uint32_t       nof_teids     = 0;
  const uint32_t max_nof_teids;

  std::vector<bool> teid_pool;
};
} // namespace srsran
