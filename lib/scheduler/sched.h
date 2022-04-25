
#ifndef SRSGNB_SCHED_H
#define SRSGNB_SCHED_H

#include "cell/cell_sched.h"
#include "event_manager.h"
#include "sched_strategy/data_scheduler.h"
#include "srsgnb/scheduler/sched_interface.h"

namespace srsgnb {

class sched final : public sched_interface
{
public:
  explicit sched(sched_configuration_notifier& notifier);

  bool handle_cell_configuration_request(const cell_configuration_request_message& msg) override;

  /// Add UE.
  void handle_add_ue_request(const sched_ue_creation_request_message& ue_request) override;

  /// Reconfiguration existing UE.
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override;

  /// Delete UE.
  void handle_ue_delete_request(du_ue_index_t ue_index) override { mac_notifier.on_ue_delete_response(ue_index); }

  /// Called when RACH is detected.
  void handle_rach_indication(const rach_indication_message& msg) override;

  /// Obtain DL scheduling result.
  const dl_sched_result* get_dl_sched(slot_point sl_tx, du_cell_index_t cell_index) override;

  /// Obtain UL scheduling result.
  const ul_sched_result* get_ul_sched(slot_point sl_tx, du_cell_index_t cell_index) override;

  /// UE Scheduling Request.
  void ul_sr_info(const sr_indication_message& sr) override { pending_events.handle_sr_indication(sr); }

  /// UE UL Buffer Status Report.
  void ul_bsr(const ul_bsr_indication_message& bsr) override { pending_events.handle_ul_bsr(bsr); }

private:
  void slot_indication(slot_point sl_tx, du_cell_index_t cell_index);

  sched_configuration_notifier& mac_notifier;
  srslog::basic_logger&         logger;

  /// Repository of created UEs.
  ue_map_t ue_db;

  // Data UE scheduler
  std::unique_ptr<data_scheduler> data_sched;

  /// Cell-specific resources and schedulers.
  cell_sched_manager cells;

  event_manager pending_events;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_H
