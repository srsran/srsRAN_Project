/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amf_connection_setup_routine.h"
#include "srsran/ngap/ngap_setup.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_setup_routine::amf_connection_setup_routine(ngap_connection_manager& ngap_conn_mng_) :
  ngap_conn_mng(ngap_conn_mng_)
{
}

void amf_connection_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not ngap_conn_mng.handle_amf_tnl_connection_request()) {
    CORO_EARLY_RETURN(false);
  }

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, send_ng_setup_request());

  CORO_RETURN(std::holds_alternative<ngap_ng_setup_response>(result_msg));
}

async_task<ngap_ng_setup_result> amf_connection_setup_routine::send_ng_setup_request()
{
  // Initiate NG Setup Request.
  return ngap_conn_mng.handle_ng_setup_request(/*max_setup_retries*/ 1);
}
