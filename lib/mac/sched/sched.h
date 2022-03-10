
#ifndef SRSGNB_SCHED_H
#define SRSGNB_SCHED_H

#include "cell/cell_sched.h"
#include "sched_strategy/data_scheduler.h"
#include "srsgnb/mac/sched_interface.h"

namespace srsgnb {

class sched final : public sched_interface
{
public:
  explicit sched(sched_cfg_notifier& notifier);

  bool handle_cell_configuration_request(const cell_configuration_request_message& msg) override;

  /// Add/Configure UE
  void config_ue(rnti_t rnti) override { mac_notifier.on_ue_config_complete(rnti); }

  /// Delete UE
  void delete_ue_request(rnti_t rnti) override { mac_notifier.on_ue_delete_response(rnti); }

  /// Obtain DL scheduling result
  const dl_sched_result* get_dl_sched(slot_point sl_tx, du_cell_index_t cell_index) override;

  /// Obtain UL scheduling result
  const ul_sched_result* get_ul_sched(slot_point sl_tx, du_cell_index_t cell_index) override;

private:
  sched_cfg_notifier& mac_notifier;

  srslog::basic_logger& logger;

  // Data UE scheduler
  std::unique_ptr<data_scheduler> data_sched;

  // Cell-specific resources and schedulers
  std::vector<std::unique_ptr<cell_sched>> cells;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_H
