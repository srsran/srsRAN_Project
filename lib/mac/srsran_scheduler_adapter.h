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

#include "mac_config.h"
#include "mac_ctrl/mac_scheduler_configurator.h"
#include "mac_scheduler_adapter.h"
#include "mac_ul/mac_scheduler_ul_buffer_state_updater.h"
#include "srsran/mac/mac_configuration_helpers.h"
#include "srsran/scheduler/mac_scheduler.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {

/// \brief This class adapts srsRAN scheduler interface to operate with srsRAN MAC.
/// The configuration completion notification handling (e.g. ue creation complete) is deferred for later processing
/// rather than being processed inline. We defer the processing because we do not want it to take place while
/// the scheduler is still processing the slot_indication, given that latter has higher priority.
class srsran_scheduler_adapter final : public mac_scheduler_adapter
{
public:
  explicit srsran_scheduler_adapter(mac_common_config_t& cfg_) : cfg(cfg_), notifier(*this) {}

  void set_sched(mac_scheduler& sched_) override { srs_sched = &sched_; }

  void add_cell(const mac_cell_creation_request& msg) override;

  void remove_cell(du_cell_index_t cell_index) override
  {
    // TODO: Call scheduler cell remove.
  }

  async_task<bool> handle_ue_creation_request(const mac_ue_create_request& msg) override;

  async_task<bool> handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg) override;

  async_task<bool> handle_ue_removal_request(const mac_ue_delete_request& msg) override;

  sched_configuration_notifier& get_sched_notifier() override { return notifier; }

  void handle_ul_bsr_indication(const mac_bsr_ce_info& bsr) override;

  void handle_ul_sched_command(const mac_ul_scheduling_command& cmd) override;

private:
  class sched_config_notif_adapter : public sched_configuration_notifier
  {
  public:
    explicit sched_config_notif_adapter(srsran_scheduler_adapter& parent_) : parent(parent_) {}
    void on_ue_config_complete(du_ue_index_t ue_index) override;
    void on_ue_delete_response(du_ue_index_t ue_index) override;

  private:
    srsran_scheduler_adapter& parent;
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
