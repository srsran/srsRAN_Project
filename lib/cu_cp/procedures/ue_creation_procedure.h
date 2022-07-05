/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_UE_CREATION_PROCEDURE_H
#define SRSGNB_CU_CP_UE_CREATION_PROCEDURE_H

#include "../log_format.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class ue_creation_procedure
{
public:
  ue_creation_procedure(ue_index_t                     ue_index_candidate,
                        du_cell_index_t                pcell_index,
                        const f1ap_initial_ul_rrc_msg& init_ul_rrc_msg,
                        const cu_cp_manager_config_t&  cfg_,
                        ue_manager_ctrl_configurer&    ue_mng_) :
    cfg(cfg_), logger(cfg.logger), msg(init_ul_rrc_msg), ue_mng(ue_mng_)
  {
    ue_ctx.ue_index    = ue_index_candidate;
    ue_ctx.pcell_index = pcell_index;
    ue_ctx.c_rnti      = to_rnti(init_ul_rrc_msg.msg->c_rnti.value);
  }

  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);

    log_proc_started(logger, ue_ctx.ue_index, ue_ctx.c_rnti, "UE Create");

    // 1. Verify if UE index was successfully allocated and params are valid.
    if (ue_ctx.ue_index == MAX_NOF_UES) {
      log_proc_failure(logger, MAX_NOF_UES, ue_ctx.c_rnti, name(), "Failure to allocate UE index.");
      CORO_EARLY_RETURN();
    }
    if (ue_mng.find_rnti(ue_ctx.c_rnti) != nullptr) {
      log_proc_failure(logger, MAX_NOF_UES, ue_ctx.c_rnti, name(), "Repeated RNTI.");
      CORO_EARLY_RETURN();
    }

    // 2. Create UE contex.
    if (ue_mng.add_ue(std::move(ue_ctx)) == nullptr) {
      log_proc_failure(logger, ue_ctx.ue_index, ue_ctx.c_rnti, "UE failed to be created in DU manager.");
      clear_ue();
      CORO_EARLY_RETURN();
    }

    // 3. Create SRB0 bearer and notifier

    // TODO

    // 4. Pass container to RRC

    // TODO

    log_proc_completed(logger, ue_ctx.ue_index, ue_ctx.c_rnti, "UE Create");
    CORO_RETURN();
  }

  static const char* name() { return "UE Create"; }

private:
  void clear_ue()
  {
    // TODO
  }
  const cu_cp_manager_config_t& cfg;
  srslog::basic_logger&         logger;

  f1ap_initial_ul_rrc_msg msg;

  ue_manager_ctrl_configurer& ue_mng;

  ue_context ue_ctx{};
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_CU_CP_UE_CREATION_PROCEDURE_H
