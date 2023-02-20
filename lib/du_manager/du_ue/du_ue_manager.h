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

#include "du_ue.h"
#include "ue_manager_ctrl_configurator.h"
#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/adt/stable_id_map.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/du_manager/du_manager_params.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsran {
namespace srs_du {

class du_ue_manager : public ue_manager_ctrl_configurator
{
public:
  explicit du_ue_manager(du_manager_params& cfg_, du_ran_resource_manager& cell_res_alloc);

  void                                        handle_ue_create_request(const ul_ccch_indication_message& msg);
  async_task<f1ap_ue_context_update_response> handle_ue_config_request(const f1ap_ue_context_update_request& msg);
  async_task<void>                            handle_ue_delete_request(const f1ap_ue_delete_request& msg);

  const stable_id_map<du_ue_index_t, du_ue, MAX_NOF_DU_UES>& get_ues() { return ue_db; }

  void schedule_async_task(du_ue_index_t ue_index, async_task<void>&& task)
  {
    ue_ctrl_loop[ue_index].schedule(std::move(task));
  }

private:
  du_ue* add_ue(std::unique_ptr<du_ue> ue_ctx) override;
  du_ue* find_ue(du_ue_index_t ue_index) override;
  du_ue* find_rnti(rnti_t rnti) override;
  void   remove_ue(du_ue_index_t ue_index) override;

  du_manager_params&       cfg;
  du_ran_resource_manager& cell_res_alloc;
  srslog::basic_logger&    logger;

  stable_id_map<du_ue_index_t, du_ue, MAX_NOF_DU_UES> ue_db;
  std::array<du_ue_index_t, MAX_NOF_DU_UES>           rnti_to_ue_index;

  // task event loops indexed by ue_index
  slotted_array<async_task_sequencer, MAX_NOF_DU_UES> ue_ctrl_loop;
};

} // namespace srs_du
} // namespace srsran
