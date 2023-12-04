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

#include "../du_manager_test_helpers.h"
#include "srsran/mac/config/mac_cell_group_config_factory.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"

namespace srsran {
namespace srs_du {

class du_ue_dummy : public du_ue_controller,
                    public mac_ue_radio_link_notifier,
                    public rlc_tx_upper_layer_control_notifier
{
public:
  bool                    ue_notifiers_disconnected = false;
  optional<du_ue_index_t> last_rlf_ue_index;
  optional<rlf_cause>     last_rlf_cause;

  du_ue_dummy(std::unique_ptr<du_ue> ctx_, fifo_async_task_scheduler* ue_ctrl_loop_) :
    ctx(std::move(ctx_)), ue_ctrl_loop(ue_ctrl_loop_)
  {
  }

  std::unique_ptr<du_ue>     ctx;
  fifo_async_task_scheduler* ue_ctrl_loop;

  const du_ue&     get_context() const override { return *ctx; }
  du_ue&           get_context() override { return *ctx; }
  async_task<void> disconnect_notifiers() override
  {
    ue_notifiers_disconnected = true;
    return launch_no_op_task();
  }
  void schedule_async_task(async_task<void> task) override { ue_ctrl_loop->schedule(std::move(task)); }
  void handle_rlf_detection(rlf_cause cause) override {}
  void handle_crnti_ce_detection() override {}
  void handle_scheduling_activation(bool active) override {}
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

  du_ue* add_ue(std::unique_ptr<du_ue> u) override
  {
    du_ue* ret = u.get();
    ues.emplace(u->ue_index, std::move(u), &ue_ctrl_loop);
    return ret;
  }
  void   remove_ue(du_ue_index_t ue_index) override { ues.erase(ue_index); }
  void   update_crnti(du_ue_index_t ue_index, rnti_t rnti) override {}
  du_ue* find_ue(du_ue_index_t ue_index) override
  {
    return ues.contains(ue_index) ? &ues[ue_index].get_context() : nullptr;
  }
  const du_ue* find_ue(du_ue_index_t ue_index) const override
  {
    return ues.contains(ue_index) ? &ues[ue_index].get_context() : nullptr;
  }
  du_ue_controller* find_rnti(rnti_t rnti) override
  {
    for (auto& u : ues) {
      if (u.get_context().rnti == rnti) {
        return &u;
      }
    }
    return nullptr;
  }
  du_ue* find_f1ap_ue_id(gnb_du_ue_f1ap_id_t f1ap_ue_id) override
  {
    for (auto& u : ues) {
      if (u.get_context().f1ap_ue_id == f1ap_ue_id) {
        return &u.get_context();
      }
    }
    return nullptr;
  }
  gtpu_teid_pool& get_f1u_teid_pool() override { return teid_pool; }

  void schedule_async_task(du_ue_index_t ue_index, async_task<void> task) override
  {
    ue_ctrl_loop.schedule(std::move(task));
  }

  du_ue_controller& get_ue_controller(du_ue_index_t ue_index) override { return ues[ue_index]; }

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