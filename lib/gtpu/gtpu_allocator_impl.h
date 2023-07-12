/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gtpu/gtpu_allocator.h"
#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/support/compiler.h"
#include <vector>

namespace srsran {

class gtpu_allocator_impl final : public gtpu_allocator
{
public:
  explicit gtpu_allocator_impl(uint32_t max_teids_) : max_teids(max_teids_), teid_pool(max_teids_) {}

  SRSRAN_NODISCARD bool allocate(gtpu_teid_t& teid) override
  {
    if (full()) {
      return false;
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
      return false;
    }
    next_teid = (tmp + 1) % max_teids;
    teid      = gtpu_teid_t{tmp};
    nof_teids++;
    return true;
  }

  SRSRAN_NODISCARD bool free(gtpu_teid_t teid) override
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
