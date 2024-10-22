/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/unique_thread.h"

namespace srsran {
namespace test_helpers {

class du_high_worker_manager
{
public:
  virtual ~du_high_worker_manager() = default;

  virtual void stop() = 0;

  virtual srs_du::du_high_executor_mapper& get_exec_mapper() = 0;
};

struct du_high_worker_config {
  unsigned             nof_cell_workers   = 1;
  bool                 use_os_thread_prio = false;
  span<const unsigned> du_cell_cores;
};

/// Create DU-high executor mapper.
std::unique_ptr<du_high_worker_manager> create_multi_threaded_du_high_executor_mapper(const du_high_worker_config& cfg);

} // namespace test_helpers
} // namespace srsran
