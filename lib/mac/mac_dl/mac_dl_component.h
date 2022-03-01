
#ifndef SRSGNB_MAC_DL_COMPONENT_H
#define SRSGNB_MAC_DL_COMPONENT_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "../sched/sched.h"
#include "mac_dl_mux.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/execute_on.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/task_executor.h"

namespace srsgnb {

class mac_dl_component final : public mac_dl_configurer
{
public:
  explicit mac_dl_component(mac_common_config_t& cfg_) :
    cfg(cfg_), logger(cfg.logger), sched_cfg_notif(*this), sched_obj(sched_cfg_notif)
  {}

  async_task<bool> add_ue(const mac_ue_create_request_message& request) override
  {
    return launch_async([this, request](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);

      // 1. Change to respective DL executor
      CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));

      // 2. Insert UE and DL bearers
      mux.add_ue(request.ue_index, request.crnti, request.bearers);

      // 3. Create UE in scheduler
      log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
      sched_obj.config_ue(request.crnti);

      // 4. Await scheduler to respond via notifier
      CORO_AWAIT(sched_cfg_notif_map[request.crnti % sched_cfg_notif_map.size()]);

      log_proc_completed(logger, request.ue_index, request.crnti, "Sched UE Config");

      // 5. Change back to CTRL executor before returning
      CORO_AWAIT(execute_on(cfg.ctrl_exec));

      CORO_RETURN(true);
    });
  }

  async_task<void> remove_ue(const mac_ue_delete_request_message& request) override
  {
    return launch_async([this, request](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);

      // 1. Change to respective DL executor
      CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));

      // 2. Remove UE from scheduler
      sched_obj.delete_ue_request(request.rnti);

      // 3. Await scheduler to respond via notifier
      CORO_AWAIT(sched_cfg_notif_map[request.rnti % sched_cfg_notif_map.size()]);

      // 4. Remove UE associated DL channels
      mux.remove_ue(request.ue_index);

      // 5. Change back to CTRL executor before returning
      CORO_AWAIT(execute_on(cfg.ctrl_exec));

      CORO_RETURN();
    });
  }

  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& request) override
  {
    return launch_async([this, request](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);

      // 1. Change to respective DL executor
      CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));

      // 2. Remove UE DL bearers
      mux.remove_bearers(request.ue_index, request.bearers_to_rem);

      // 3. AddMod UE DL bearers
      mux.addmod_bearers(request.ue_index, request.bearers_to_addmod);

      // 4. Configure UE in Scheduler
      log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
      sched_obj.config_ue(request.crnti);

      // 4. Await scheduler to respond via notifier
      CORO_AWAIT(sched_cfg_notif_map[request.crnti % sched_cfg_notif_map.size()]);

      log_proc_completed(logger, request.ue_index, request.crnti, "Sched UE Config");

      // 5. Change back to CTRL executor before returning
      CORO_AWAIT(execute_on(cfg.ctrl_exec));

      CORO_RETURN(true);
    });
  }

private:
  class sched_response_adapter final : public sched_cfg_notifier
  {
    mac_dl_component& mac_ref;

  public:
    explicit sched_response_adapter(mac_dl_component& mac_) : mac_ref(mac_) {}
    void on_ue_config_complete(rnti_t rnti) override
    {
      // Note: Delay response for now to simulate scheduler async behavior
      mac_ref.cfg.dl_execs[0]->execute(
          [this, rnti]() { mac_ref.sched_cfg_notif_map[rnti % mac_ref.sched_cfg_notif_map.size()].set(); });
    }
    void on_ue_delete_response(rnti_t rnti) override { on_ue_config_complete(rnti); }
  };

  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;

  // TODO: make the sched notification cleaner
  std::array<manual_event_flag, MAX_NOF_UES> sched_cfg_notif_map;
  sched_response_adapter                     sched_cfg_notif;

  mac_dl_mux mux;
  sched      sched_obj;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_COMPONENT_H
