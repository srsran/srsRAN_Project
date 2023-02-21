/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "mac_scheduler_configurator.h"
#include "srsran/mac/mac_configuration_helpers.h"
#include "srsran/scheduler/mac_scheduler.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {

/// \brief This class adapts SRS scheduler interface to operate with srs MAC.
/// The configuration completion notification handling (e.g. ue creation complete) is deferred for later processing
/// rather than being processed inline. We defer the processing because we do not want it to take place while
/// the scheduler is still processing the slot_indication, given that latter has higher priority.
class srs_sched_config_adapter final : public mac_scheduler_configurator
{
public:
  srs_sched_config_adapter(mac_common_config_t& cfg_) : cfg(cfg_), notifier(*this) {}

  void set_sched(mac_scheduler& sched_) { srs_sched = &sched_; }

  void add_cell(const mac_cell_creation_request& msg) override
  {
    srs_sched->handle_cell_configuration_request(msg.sched_req);
  }

  void remove_cell(du_cell_index_t cell_index) override
  {
    // TODO: Call scheduler cell remove.
  }

  async_task<bool> handle_ue_creation_request(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);

      // Create UE in the Scheduler.
      srs_sched->handle_ue_creation_request(make_scheduler_ue_creation_request(msg));

      // Await Scheduler notification that UE was added.
      CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
      sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

      CORO_RETURN(true);
    });
  }

  async_task<bool> handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);

      // Reconfigure UE in the scheduler.
      srs_sched->handle_ue_reconfiguration_request(make_scheduler_ue_reconfiguration_request(msg));

      // Await Scheduler notification.
      CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
      sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

      CORO_RETURN(true);
    });
  }

  async_task<bool> handle_ue_removal_request(const mac_ue_delete_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);

      // Remove UE from the scheduler.
      srs_sched->handle_ue_removal_request(msg.ue_index);

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

      // Remove continuation of task in ctrl executor.
      parent.cfg.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); });
    }
    void on_ue_delete_response(du_ue_index_t ue_index) override
    {
      srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);

      // Remove continuation of task in ctrl executor.
      parent.cfg.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); });
    }

  private:
    srs_sched_config_adapter& parent;
  };

  mac_common_config_t& cfg;

  /// srsGNB scheduler.
  mac_scheduler* srs_sched = nullptr;

  /// Notifier that is used by MAC to start and await configurations of the scheduler.
  sched_config_notif_adapter notifier;

  /// List of event flags used by scheduler to notify that the configuration is complete.
  struct ue_notification_context {
    manual_event<bool> ue_config_ready;
  };
  std::array<ue_notification_context, MAX_NOF_DU_UES> sched_cfg_notif_map;
};

} // namespace srsran
