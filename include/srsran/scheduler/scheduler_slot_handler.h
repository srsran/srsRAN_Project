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

#include "srsran/ran/du_types.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

struct sched_result;

class scheduler_slot_handler
{
public:
  /// \brief Effect that the errors in the lower layers had on the results provided by the scheduler for a given
  /// slot and cell.
  struct error_outcome {
    bool pdcch_discarded : 1;
    bool pdsch_discarded : 1;
    bool pusch_and_pucch_discarded : 1;
  };

  virtual ~scheduler_slot_handler() = default;

  /// \brief Handle slot indications that arrive to the scheduler for a given cell.
  virtual const sched_result& slot_indication(slot_point sl_tx, du_cell_index_t cell_index) = 0;

  /// \brief Handle error indications caused by lates or invalid scheduling results.
  virtual void handle_error_indication(slot_point sl_tx, du_cell_index_t cell_index, error_outcome event) = 0;
};

} // namespace srsran
