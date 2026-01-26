/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/adt/moodycamel_mpmc_queue.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/srslog/logger.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

class gtpu_teid_pool_impl final : public gtpu_teid_pool
{
public:
  explicit gtpu_teid_pool_impl(uint32_t max_nof_teids_) :
    max_nof_teids(max_nof_teids_), teid_pool(max_nof_teids_), logger(srslog::fetch_basic_logger("GTPU"))
  {
    for (uint32_t t = GTPU_TEID_MIN.value(); t < GTPU_TEID_MIN.value() + max_nof_teids; t++) {
      if (not teid_pool.try_push(gtpu_teid_t{t})) {
        logger.warning("Failed to reserve TEID. teid={}", t);
      }
    }
  }

  [[nodiscard]] expected<gtpu_teid_t> request_teid() override
  {
    expected<gtpu_teid_t> teid = make_unexpected(default_error_t{});
    gtpu_teid_t           t;

    if (teid_pool.try_pop(t)) {
      teid = t;
    } else {
      logger.warning("Failed to allocate TEID. teid_pool_size={}", teid_pool.size());
    }

    return teid;
  }

  [[nodiscard]] bool release_teid(gtpu_teid_t teid) override
  {
    if (not teid_pool.try_push(teid)) {
      logger.warning("Failed to release TEID. teid_pool_size={}, teid={}", teid_pool.size(), teid);
      return false;
    }
    return true;
  }

  [[nodiscard]] bool full() const override { return teid_pool.empty(); }

  uint32_t get_max_nof_teids() override { return max_nof_teids; }

private:
  const uint32_t max_nof_teids;

  concurrent_queue<gtpu_teid_t,
                   concurrent_queue_policy::moodycamel_lockfree_mpmc,
                   concurrent_queue_wait_policy::non_blocking>
      teid_pool;

  srslog::basic_logger& logger;
};
} // namespace srsran
