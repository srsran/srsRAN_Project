/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_external_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include <memory>

using namespace srsran;
using namespace asn1::e2ap;

e2_external_impl::e2_external_impl(std::unique_ptr<e2_interface> decorated_e2_iface_, task_executor& task_exec_) :
  logger(srslog::fetch_basic_logger("E2")),
  decorated_e2_iface(std::move(decorated_e2_iface_)),
  task_exec(task_exec_),
  main_ctrl_loop(128)
{
}

void e2_external_impl::start()
{
  // TODO: currently created and filled here, but need to me moved to handle_e2_setup_request
  e2_setup_request_message request{};
  e2_message               e2_msg = {};
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_E2SETUP);
  e2_msg.pdu.init_msg().value.e2setup_request()->transaction_id.value.value = 1;
  request.request = e2_msg.pdu.init_msg().value.e2setup_request();

  if (not task_exec.execute([this, &request]() {
        main_ctrl_loop.schedule([this, &request](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Send E2AP Setup Request and await for E2AP setup response.
          CORO_AWAIT(decorated_e2_iface->handle_e2_setup_request(request));

          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to initiate E2AP setup procedure");
  }
}

void e2_external_impl::stop()
{
  // placeholder_
}

async_task<e2_setup_response_message> e2_external_impl::handle_e2_setup_request(e2_setup_request_message& request)
{
  return decorated_e2_iface->handle_e2_setup_request(request);
}

void e2_external_impl::handle_connection_loss()
{
  task_exec.execute([this]() { decorated_e2_iface->handle_connection_loss(); });
}

void e2_external_impl::handle_message(const e2_message& msg)
{
  task_exec.execute([this, &msg]() { decorated_e2_iface->handle_message(msg); });
}
