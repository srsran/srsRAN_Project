
#ifndef SRSGNB_MAC_DL_PROCESSOR_H
#define SRSGNB_MAC_DL_PROCESSOR_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "mac_dl_cell_processor.h"
#include "mac_dl_ue_manager.h"
#include "srs_sched_config_adapter.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_cell_manager.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/support/async/eager_async_task.h"
#include "srsgnb/support/async/execute_on.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

class mac_dl_processor final : public mac_dl_configurer, public mac_cell_manager
{
public:
  explicit mac_dl_processor(mac_common_config_t&    cfg_,
                            mac_sched_configurator& sched_cfg_,
                            mac_scheduler&          sched_,
                            du_rnti_table&          rnti_table_);

  bool has_cell(du_cell_index_t cell_index) const;

  /// Adds new cell configuration to MAC DL. The configuration is forwarded to the scheduler.
  void add_cell(const mac_cell_creation_request& cell_cfg) override;

  /// Removes cell configuration from MAC DL. The cell is also removed from the scheduler.
  void remove_cell(du_cell_index_t cell_index) override;

  mac_cell_controller& get_cell_manager(du_cell_index_t cell_index) override { return *cells[cell_index]; }

  /// Creates new UE DL context, updates logical channel MUX, adds UE in scheduler.
  async_task<bool> add_ue(const mac_ue_create_request_message& request) override;

  /// Deletes UE context in MAC DL and scheduler.
  async_task<void> remove_ue(const mac_ue_delete_request_message& request) override;

  /// Reconfigures UE in MAC DL and in the scheduler.
  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& request) override;

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) { return *cells[cell_index]; }

private:
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;

  std::array<std::unique_ptr<mac_dl_cell_processor>, MAX_NOF_DU_CELLS> cells;

  mac_sched_configurator& sched_cfg;

  mac_dl_ue_manager ue_mng;

  mac_scheduler& sched_obj;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_PROCESSOR_H
