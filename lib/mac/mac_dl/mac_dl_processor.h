
#ifndef SRSGNB_MAC_DL_PROCESSOR_H
#define SRSGNB_MAC_DL_PROCESSOR_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "../sched/sched.h"
#include "mac_cell_manager.h"
#include "mac_dl_mux.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_dl_result.h"
#include "srsgnb/mac/mac_resource_manager.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/execute_on.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

class mac_dl_processor final : public mac_dl_configurer
{
public:
  explicit mac_dl_processor(mac_common_config_t& cfg_) :
    cfg(cfg_), logger(cfg.logger), sched_cfg_notif(*this), sched_obj(sched_cfg_notif)
  {}

  /// Adds new cell configuration to MAC DL. The configuration is forwarded to the scheduler.
  void add_cell(const mac_cell_configuration& cell_cfg);

  /// Removes cell configuration from MAC DL. The cell is also removed from the scheduler.
  void remove_cell(du_cell_index_t cell_index);

  /// Starts configured cell.
  async_task<void> start_cell(du_cell_index_t cell_index);

  /// Stops configured cell.
  async_task<void> stop_cell(du_cell_index_t cell_index);

  /// Creates new UE DL context, updates logical channel MUX, adds UE in scheduler.
  async_task<bool> add_ue(const mac_ue_create_request_message& request) override;

  /// Deletes UE context in MAC DL and scheduler.
  async_task<void> remove_ue(const mac_ue_delete_request_message& request) override;

  /// Reconfigures UE in MAC DL and in the scheduler.
  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& request) override;

  sched_interface& get_sched() { return sched_obj; }

  /// \brief Processing of a new Slot. The following tasks take place:
  /// - The scheduler generates the scheduling grants for the provided slot and cell.
  /// - The scheduling grants are passed to the PHY.
  /// - The MAC DL PDUs are generated and passed to the PHY as well.
  void slot_indication(slot_point sl_tx, du_cell_index_t cell_index);

private:
  class sched_response_adapter final : public sched_cfg_notifier
  {
    mac_dl_processor& mac_ref;

  public:
    explicit sched_response_adapter(mac_dl_processor& mac_) : mac_ref(mac_) {}
    void on_ue_config_complete(rnti_t rnti) override
    {
      // Note: Delay response for now to simulate scheduler async behavior
      mac_ref.cfg.dl_execs[0]->execute(
          [this, rnti]() { mac_ref.sched_cfg_notif_map[rnti % mac_ref.sched_cfg_notif_map.size()].set(); });
    }
    void on_ue_delete_response(rnti_t rnti) override { on_ue_config_complete(rnti); }
  };

  /// Assemble struct that is going to be passed down to PHY with the DL scheduling result.
  /// \remark FAPI will use this struct to generate a DL_TTI.Request.
  void assemble_dl_sched_request(slot_point             sl_tx,
                                 du_cell_index_t        cell_index,
                                 const dl_sched_result& dl_res,
                                 mac_dl_sched_result&   mac_res);

  /// Assemble struct with the MAC PDUs of a given slot and cell that is going be passed to the PHY.
  /// \remark FAPI will use this struct to generate a Tx_Data.Request.
  void assemble_dl_data_request(slot_point             sl_tx,
                                du_cell_index_t        cell_index,
                                const dl_sched_result& dl_res,
                                mac_dl_data_result&    mac_data);

  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;

  mac_cell_manager cells;

  // TODO: make the sched notification cleaner
  std::array<manual_event_flag, MAX_NOF_UES> sched_cfg_notif_map;
  sched_response_adapter                     sched_cfg_notif;

  mac_dl_mux mux;
  sched      sched_obj;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_PROCESSOR_H
