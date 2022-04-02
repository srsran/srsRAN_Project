
#ifndef SRSGNB_SCHED_H
#define SRSGNB_SCHED_H

#include "cell/cell_sched.h"
#include "event_manager.h"
#include "sched_strategy/data_scheduler.h"
#include "srsgnb/mac/sched_interface.h"

namespace srsgnb {

class sched final : public sched_interface
{
public:
  explicit sched(sched_cfg_notifier& notifier);

  bool handle_cell_configuration_request(const cell_configuration_request_message& msg) override;

  /// Add/Configure UE.
  void config_ue(rnti_t rnti) override;

  /// Delete UE.
  void delete_ue_request(rnti_t rnti) override { mac_notifier.on_ue_delete_response(rnti); }

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

  sched_cfg_notifier&   mac_notifier;
  srslog::basic_logger& logger;

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
