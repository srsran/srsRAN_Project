/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../pucch_scheduling/pucch_allocator.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "ue.h" // TEMP

namespace srsgnb {

class pdcch_resource_allocator;
struct cell_resource_allocator;

struct ue_scheduler_cell_params {
  du_cell_index_t           cell_index;
  pdcch_resource_allocator* pdcch_sched;
  pucch_allocator*          pucch_alloc;
  cell_resource_allocator*  cell_res_alloc;
  unsigned                  max_msg4_mcs_index;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler
{
public:
  virtual ~ue_scheduler() = default;

  virtual void add_cell(const ue_scheduler_cell_params& params) = 0;

  /// Schedule UE DL grants for a given {slot, cell}.
  virtual void run_slot(slot_point slot_tx, du_cell_index_t cell_index) = 0;

  /// Return UE configurator.
  virtual scheduler_ue_configurator& get_ue_configurator() = 0;

  virtual scheduler_feedback_handler& get_feedback_handler() = 0;

  virtual scheduler_dl_buffer_state_indication_handler& get_dl_buffer_state_indication_handler() = 0;
};

} // namespace srsgnb
