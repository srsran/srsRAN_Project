/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UE_SCHEDULER_H
#define SRSGNB_UE_SCHEDULER_H

#include "srsgnb/scheduler/scheduler_configurator.h"
#include "ue.h" // TEMP

namespace srsgnb {

class pdcch_scheduler;
class cell_resource_allocator;

struct ue_scheduler_cell_params {
  du_cell_index_t          cell_index;
  pdcch_scheduler*         pdcch_sched;
  cell_resource_allocator* cell_res_alloc;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler : public scheduler_ue_configurator
{
public:
  virtual void add_cell(const ue_scheduler_cell_params& params) = 0;

  /// Schedule UE DL grants for a given {slot, cell}.
  virtual void run_slot(slot_point slot_tx, du_cell_index_t cell_index) = 0;

  /// TEMP
  virtual ue_list& get_ue_list() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_UE_SCHEDULER_H
