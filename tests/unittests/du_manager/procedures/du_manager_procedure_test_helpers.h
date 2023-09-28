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

class ue_manager_dummy : public du_ue_manager_repository
{
  dummy_teid_pool teid_pool;

public:
  slotted_array<std::unique_ptr<du_ue>, MAX_NOF_DU_UES> ues;

  optional<du_ue_index_t> last_rlf_ue_index;
  optional<rlf_cause>     last_rlf_cause;

  du_ue* add_ue(std::unique_ptr<du_ue> u) override
  {
    du_ue* ret = u.get();
    ues.emplace(u->ue_index, std::move(u));
    return ret;
  }
  void   remove_ue(du_ue_index_t ue_index) override { ues.erase(ue_index); }
  void   update_crnti(du_ue_index_t ue_index, rnti_t rnti) override {}
  du_ue* find_ue(du_ue_index_t ue_index) override { return ues.contains(ue_index) ? ues[ue_index].get() : nullptr; }
  du_ue* find_rnti(rnti_t rnti) override
  {
    for (auto& u : ues) {
      if (u->rnti == rnti) {
        return u.get();
      }
    }
    return nullptr;
  }
  du_ue* find_f1ap_ue_id(gnb_du_ue_f1ap_id_t f1ap_ue_id) override
  {
    for (auto& u : ues) {
      if (u->f1ap_ue_id == f1ap_ue_id) {
        return u.get();
      }
    }
    return nullptr;
  }
  void handle_radio_link_failure(du_ue_index_t ue_index, rlf_cause cause) override
  {
    last_rlf_ue_index = ue_index;
    last_rlf_cause    = cause;
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