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
                                du_high_ue_executor_mapper&   ue_exec_mapper_,
                                task_executor&                ctrl_exec_) :
    msg(msg_), ues(ues_), ue_exec_mapper(ue_exec_mapper_), ctrl_exec(ctrl_exec_)
  {
  }

  void operator()(coro_context<async_task<f1ap_ue_create_response>>& ctx)
  {
    CORO_BEGIN(ctx);

    CORO_AWAIT(execute_on(ue_exec_mapper.executor(msg.ue_index)));

    ues.add_ue(msg.ue_index);

    CORO_AWAIT(execute_on(ctrl_exec));

    CORO_RETURN(create_response());
  }

private:
  f1ap_ue_create_response create_response()
  {
    f1ap_ue_create_response resp{};
    resp.result = true;
    return resp;
  }

  f1ap_ue_create_request      msg;
  f1ap_du_ue_manager&         ues;
  du_high_ue_executor_mapper& ue_exec_mapper;
  task_executor&              ctrl_exec;
};

} // namespace srs_du
} // namespace srsgnb
