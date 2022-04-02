
#ifndef SRSGNB_SCHED_CONFIG_ADAPTER_H
#define SRSGNB_SCHED_CONFIG_ADAPTER_H

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/mac/sched_configurer.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/manual_event.h"

namespace srsgnb {

/// Class that adapts scheduler configuration completion notifications implemented via "sched_cfg_notifier" to
/// methods that return awaitables.
class sched_config_adapter
{
public:
  sched_config_adapter() : notifier(*this) {}

  async_task<bool> ue_configuration_completed(rnti_t rnti)
  {
    return launch_async([this, rnti](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(sched_cfg_notif_map[rnti].ue_config_ready);
      sched_cfg_notif_map[rnti].ue_config_ready.reset();
      CORO_RETURN(true);
    });
  }

  async_task<bool> ue_deletion_completed(rnti_t rnti)
  {
    return launch_async([this, rnti](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(sched_cfg_notif_map[rnti].ue_delete_ready);
      sched_cfg_notif_map[rnti].ue_delete_ready.reset();
      CORO_RETURN(true);
    });
  }

  sched_cfg_notifier& get_notifier() { return notifier; }

private:
  class sched_config_notif_adapter : public sched_cfg_notifier
  {
  public:
    sched_config_notif_adapter(sched_config_adapter& parent_) : parent(parent_) {}
    void on_ue_config_complete(rnti_t rnti) override { parent.sched_cfg_notif_map[rnti].ue_config_ready.set(true); }
    void on_ue_delete_response(rnti_t rnti) override { parent.sched_cfg_notif_map[rnti].ue_delete_ready.set(true); }

  private:
    sched_config_adapter& parent;
  };

  /// Notifier that is used by MAC to start and await configurations of the scheduler.
  sched_config_notif_adapter notifier;

  /// List of event flags used by scheduler to notify that the configuration is complete.
  struct rnti_context {
    manual_event<bool> ue_config_ready;
    manual_event<bool> ue_delete_ready;
  };
  circular_array<rnti_context, MAX_NOF_UES> sched_cfg_notif_map;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_CONFIG_ADAPTER_H
