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

#include "srsgnb/cu_cp/cu_up_processor.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class initial_cu_up_processor_setup_procedure
{
public:
  initial_cu_up_processor_setup_procedure(cu_up_processor_context&                   context_,
                                          e1_connection_manager&                     e1_conn_mng_,
                                          cu_up_processor_cu_up_management_notifier& cu_cp_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<cu_cp_e1_setup_response_message> start_cu_cp_e1_setup_request();
  void handle_cu_cp_e1_setup_response(const asn1::e1ap::gnb_cu_cp_e1_setup_resp_s& resp);

  cu_up_processor_context& context;

  e1_connection_manager&                     e1_conn_mng;
  cu_up_processor_cu_up_management_notifier& cu_cp_notifier;

  cu_cp_e1_setup_response_message response_msg = {};
};

} // namespace srs_cu_cp
} // namespace srsgnb
