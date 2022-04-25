
#ifndef SRSGNB_SRS_SCHED_CONFIG_ADAPTER_H
#define SRSGNB_SRS_SCHED_CONFIG_ADAPTER_H

#include "mac_sched_configurator.h"
#include "sched_config_helpers.h"
#include "srsgnb/adt/circular_array.h"
#include "srsgnb/scheduler/sched_configurator.h"
#include "srsgnb/support/async/eager_async_task.h"
#include "srsgnb/support/async/manual_event.h"

namespace srsgnb {

/// \brief This class adapts SRS scheduler interface to operate with srs MAC.
/// The configuration completion notification handling (e.g. ue creation complete) is deferred for later processing
/// rather than being processed inline. We defer the processing because we do not want it to take place while
/// the scheduler is still processing the slot_indication, given that latter has higher priority.
class srs_sched_config_adapter final : public mac_sched_configurator
{
public:
  srs_sched_config_adapter(mac_common_config_t& cfg_) : cfg(cfg_), notifier(*this) {}

  void set_sched(sched_configurator& sched_) { srs_sched = &sched_; }

  async_task<bool> handle_ue_creation_request(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);

      setup_new_configuration(msg.cell_index, msg.ue_index);

      // Create UE in the Scheduler.
      srs_sched->handle_add_ue_request(make_ue_creation_request(msg));

      // Await Scheduler notification that UE was added.
      CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
      sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

      CORO_RETURN(true);
    });
  }

  async_task<bool> handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);

      setup_new_configuration(msg.cell_index, msg.ue_index);

      // Reconfigure UE in the scheduler.
      srs_sched->handle_ue_reconfiguration_request(make_ue_reconfiguration_request(msg));

      // Await Scheduler notification.
      CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
      sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

      CORO_RETURN(true);
    });
  }

  async_task<bool> handle_ue_deletion_request(const mac_ue_delete_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);

      setup_new_configuration(msg.cell_index, msg.ue_index);

      // Remove UE from the scheduler.
      srs_sched->handle_ue_delete_request(msg.ue_index);

      // Await Scheduler notification.
      CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
      sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();
      CORO_RETURN(true);
    });
  }

  sched_configuration_notifier& get_sched_notifier() override { return notifier; }

private:
  class sched_config_notif_adapter : public sched_configuration_notifier
  {
  public:
    explicit sched_config_notif_adapter(srs_sched_config_adapter& parent_) : parent(parent_) {}
    void on_ue_config_complete(du_ue_index_t ue_index) override
    {
      srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);
      parent.sched_cfg_notif_map[ue_index].dl_exec->defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); });
    }
    void on_ue_delete_response(du_ue_index_t ue_index) override
    {
      srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);
      parent.sched_cfg_notif_map[ue_index].dl_exec->defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); });
    }

  private:
    srs_sched_config_adapter& parent;
  };

  /// Sets the task executor from where the scheduler notification is going to be processed.
  void setup_new_configuration(du_cell_index_t cell_index, du_ue_index_t ue_index)
  {
    // Set the task executor from where the scheduler notification is going to be processed.
    sched_cfg_notif_map[ue_index].dl_exec = cfg.dl_execs[cell_index % cfg.dl_execs.size()];

    // Reset notification event state.
    sched_cfg_notif_map[ue_index].ue_config_ready.reset();
  }

  mac_common_config_t& cfg;

  /// srsGNB scheduler.
  sched_configurator* srs_sched = nullptr;

  /// Notifier that is used by MAC to start and await configurations of the scheduler.
  sched_config_notif_adapter notifier;

  /// List of event flags used by scheduler to notify that the configuration is complete.
  struct ue_notification_context {
    task_executor*     dl_exec = nullptr;
    manual_event<bool> ue_config_ready;
  };
  std::array<ue_notification_context, MAX_NOF_UES> sched_cfg_notif_map;
};

} // namespace srsgnb

#endif // SRSGNB_SRS_SCHED_CONFIG_ADAPTER_H
