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

#include "srsran/adt/moodycamel_bounded_mpmc_queue.h"
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
                   concurrent_queue_policy::moodycamel_lockfree_bounded_mpmc,
                   concurrent_queue_wait_policy::non_blocking>
      teid_pool;

  srslog::basic_logger& logger;
};
} // namespace srsran
