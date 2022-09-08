/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../f1ap_du_ue.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {
namespace srs_du {

class f1ap_du_ue_creation_procedure
{
public:
  f1ap_du_ue_creation_procedure(const f1ap_ue_create_request& msg_,
                                f1ap_du_ue_manager&           ues_,
                                const f1ap_du_context&        du_ctx_,
                                du_high_ue_executor_mapper&   ue_exec_mapper_,
                                task_executor&                ctrl_exec_,
                                f1c_message_notifier&         f1c_notifier_,
                                f1ap_event_manager&           ev_mng_) :
    msg(std::move(msg_)),
    ues(ues_),
    du_ctx(du_ctx_),
    ue_exec_mapper(ue_exec_mapper_),
    ctrl_exec(ctrl_exec_),
    f1c_notifier(f1c_notifier_),
    ev_mng(ev_mng_)
  {
  }

  void operator()(coro_context<async_task<f1ap_ue_create_response>>& ctx)
  {
    CORO_BEGIN(ctx);

    CORO_AWAIT(execute_on(ue_exec_mapper.executor(msg.ue_index)));

    add_ue_to_f1();

    CORO_AWAIT(execute_on(ctrl_exec));

    CORO_RETURN(create_response());
  }

private:
  void add_ue_to_f1()
  {
    f1ap_du_ue& u = ues.add_ue(msg.ue_index);

    // Create F1c bearer for SRB0.
    auto& ue_pcell = du_ctx.served_cells[msg.cell_index];
    auto  srb0     = std::make_unique<f1c_srb0_du_bearer>(u.gnb_du_ue_f1ap_id,
                                                     msg.c_rnti,
                                                     ue_pcell.served_cell_info.nrcgi,
                                                     std::move(msg.du_cu_rrc_container),
                                                     f1c_notifier,
                                                     ev_mng);
    u.bearers.push_back(std::move(srb0));
  }

  f1ap_ue_create_response create_response()
  {
    f1ap_ue_create_response resp{};
    resp.result = true;
    for (std::unique_ptr<f1_du_bearer>& bearer : ues[msg.ue_index].bearers) {
      resp.bearers_added.push_back(bearer.get());
    }
    return resp;
  }

  f1ap_ue_create_request      msg;
  f1ap_du_ue_manager&         ues;
  const f1ap_du_context&      du_ctx;
  du_high_ue_executor_mapper& ue_exec_mapper;
  task_executor&              ctrl_exec;
  f1c_message_notifier&       f1c_notifier;
  f1ap_event_manager&         ev_mng;
};

} // namespace srs_du
} // namespace srsgnb
