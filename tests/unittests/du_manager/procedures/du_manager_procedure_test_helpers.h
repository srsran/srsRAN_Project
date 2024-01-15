/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../du_manager_test_helpers.h"
#include "srsran/mac/config/mac_cell_group_config_factory.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"

namespace srsran {
namespace srs_du {

class du_ue_dummy : public du_ue, public mac_ue_radio_link_notifier, public rlc_tx_upper_layer_control_notifier
{
public:
  bool                    ue_notifiers_disconnected = false;
  optional<du_ue_index_t> last_rlf_ue_index;
  optional<rlf_cause>     last_rlf_cause;

  du_ue_dummy(const du_ue_context&         ctx_,
              ue_ran_resource_configurator resources_,
              fifo_async_task_scheduler*   ue_ctrl_loop_) :
    du_ue(ctx_, std::move(resources_)), ue_ctrl_loop(ue_ctrl_loop_)
  {
  }

  fifo_async_task_scheduler* ue_ctrl_loop;

  async_task<void> disconnect_notifiers() override
  {
    ue_notifiers_disconnected = true;
    return launch_no_op_task();
  }
  void schedule_async_task(async_task<void> task) override { ue_ctrl_loop->schedule(std::move(task)); }
  void handle_rlf_detection(rlf_cause cause) override {}
  void handle_crnti_ce_detection() override {}
  void stop_drb_traffic() override {}
  mac_ue_radio_link_notifier&          get_mac_rlf_notifier() override { return *this; }
  void                                 on_rlf_detected() override {}
  void                                 on_crnti_ce_received() override {}
  rlc_tx_upper_layer_control_notifier& get_rlc_rlf_notifier() override { return *this; }
  void                                 on_protocol_failure() override {}
  void                                 on_max_retx() override {}
};

class ue_manager_dummy : public du_ue_manager_repository
{
  dummy_teid_pool teid_pool;

public:
  slotted_array<du_ue_dummy, MAX_NOF_DU_UES> ues;

  du_ue* add_ue(const du_ue_context& u, ue_ran_resource_configurator resources) override
  {
    du_ue_index_t ue_index = u.ue_index;
    ues.emplace(ue_index, std::move(u), std::move(resources), &ue_ctrl_loop);
    return &ues[ue_index];
  }
  void         remove_ue(du_ue_index_t ue_index) override { ues.erase(ue_index); }
  void         update_crnti(du_ue_index_t ue_index, rnti_t rnti) override {}
  du_ue*       find_ue(du_ue_index_t ue_index) override { return ues.contains(ue_index) ? &ues[ue_index] : nullptr; }
  const du_ue* find_ue(du_ue_index_t ue_index) const override
  {
    return ues.contains(ue_index) ? &ues[ue_index] : nullptr;
  }
  du_ue* find_rnti(rnti_t rnti) override
  {
    for (auto& u : ues) {
      if (u.rnti == rnti) {
        return &u;
      }
    }
    return nullptr;
  }
  du_ue* find_f1ap_ue_id(gnb_du_ue_f1ap_id_t f1ap_ue_id) override
  {
    for (auto& u : ues) {
      if (u.f1ap_ue_id == f1ap_ue_id) {
        return &u;
      }
    }
    return nullptr;
  }
  gtpu_teid_pool& get_f1u_teid_pool() override { return teid_pool; }

  void schedule_async_task(du_ue_index_t ue_index, async_task<void> task) override
  {
    ue_ctrl_loop.schedule(std::move(task));
  }

  fifo_async_task_scheduler ue_ctrl_loop{128};
};

ul_ccch_indication_message create_test_ul_ccch_message(rnti_t rnti);

class du_manager_proc_tester : public du_manager_test_bench
{
public:
  du_manager_proc_tester(span<const du_cell_config> cells_) : du_manager_test_bench(cells_) {}

  du_ue& create_ue(du_ue_index_t ue_index);

  /// \brief Run DU UE configuration procedure to completion.
  f1ap_ue_context_update_response configure_ue(const f1ap_ue_context_update_request& req);

  ue_manager_dummy ue_mng;
};

} // namespace srs_du
} // namespace srsran