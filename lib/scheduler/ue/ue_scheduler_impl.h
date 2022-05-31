/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UE_SCHEDULER_IMPL_H
#define SRSGNB_UE_SCHEDULER_IMPL_H

#include "../sched_strategy/data_scheduler.h"
#include "../support/slot_event_list.h"
#include "../support/slot_sync_point.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/adt/unique_function.h"
#include "ue_event_manager.h"
#include "ue_scheduler.h"

namespace srsgnb {

/// \brief Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot.
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler_impl final : public ue_scheduler
{
public:
  explicit ue_scheduler_impl(sched_configuration_notifier& mac_notif);

  void add_cell(const ue_scheduler_cell_params& params) override;

  /// Schedule UE DL grants for a given {slot, cell}.
  void run_slot(slot_point slot_tx, du_cell_index_t cell_index) override;

  scheduler_ue_configurator& get_ue_configurator() override { return event_mng; }

  scheduler_feedback_handler& get_feedback_handler() override { return event_mng; }

private:
  void run_sched_strategy();

  /// Repository of created UEs.
  ue_list ue_db;

  /// Processor of UE input events.
  ue_event_manager event_mng;

  /// Currently added cells.
  slot_array<ue_scheduler_cell_params, MAX_NOF_DU_CELLS> cells;

  /// Mutex used to lock carriers for joint carrier scheduling.
  slot_sync_point sync_point;
};

} // namespace srsgnb

#endif // SRSGNB_UE_SCHEDULER_IMPL_H
