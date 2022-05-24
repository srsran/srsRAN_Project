/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_impl.h"
#include "../ran/gnb_format.h"
#include "f1_procedure_asn1_helpers.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

f1ap_du_impl::f1ap_du_impl(f1c_message_handler& f1c_pdu_handler_) :
  logger(srslog::fetch_basic_logger("F1AP")), f1c(f1c_pdu_handler_)
{}

async_task<f1_setup_response_message> f1ap_du_impl::handle_f1ap_setup_request(const f1_setup_request_message& request)
{
  asn1::f1ap::f1_setup_request_s request_asn1;
  fill_asn1_f1_setup_request(request_asn1, request);

  // TODO: add procedure implementation
  return launch_async([this, res_pdu = asn1::f1ap::f1_ap_pdu_c{}, res = f1_setup_response_message{}, request](
                          coro_context<async_task<f1_setup_response_message> >& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(res_pdu, pdu_queue);

    if (res_pdu.type() == asn1::f1ap::f1_ap_pdu_c::types_opts::successful_outcome) {
      logger.info("Received F1AP PDU with successful outcome.");
      res.msg     = res_pdu.successful_outcome().value.f1_setup_resp();
      res.success = true;
    }

    if (res_pdu.type() == asn1::f1ap::f1_ap_pdu_c::types_opts::unsuccessful_outcome) {
      logger.info("Received F1AP PDU with unsuccessful outcome.");
      if (f1_setup_retry_no < request.setup_params.max_setup_retries) {
        CORO_AWAIT_VALUE(res, handle_f1_setup_failure(request, res_pdu.unsuccessful_outcome().value.f1_setup_fail()));
      } else {
        logger.error("Reached maximum number of F1 Setup connection retries ({}).",
                     request.setup_params.max_setup_retries);
        res.success = false;
      }
    }

    CORO_RETURN(res);
  });
}

async_task<f1ap_du_ue_create_response> f1ap_du_impl::handle_ue_creation_request(const f1ap_du_ue_create_request& msg)
{
  // TODO: add UE create procedure
  // TODO: Change execution context.
  return launch_async([this, msg, resp = f1ap_du_ue_create_response{}](
                          coro_context<async_task<f1ap_du_ue_create_response> >& ctx) mutable {
    CORO_BEGIN(ctx);
    resp.result = false;
    if (not ctxt.ue_ctxt_manager.contains(msg.ue_index)) {
      ctxt.ue_ctxt_manager.emplace(msg.ue_index);
      ctxt.ue_ctxt_manager[msg.ue_index].crnti             = INVALID_RNTI; // Needed?
      ctxt.ue_ctxt_manager[msg.ue_index].gnb_du_f1ap_ue_id = msg.ue_index;
      ctxt.ue_ctxt_manager[msg.ue_index].gnb_cu_f1ap_ue_id = (du_ue_index_t)-1; // TODO

      resp.result = true;
    }
    CORO_RETURN(resp);
  });
}

async_task<f1_setup_response_message> f1ap_du_impl::handle_f1_setup_failure(const f1_setup_request_message&    request,
                                                                            const asn1::f1ap::f1_setup_fail_s& failure)
{
  return launch_async([this, res = f1_setup_response_message{}, request, failure](
                          coro_context<async_task<f1_setup_response_message> >& ctx) mutable {
    CORO_BEGIN(ctx);

    if (failure->time_to_wait_present) {
      f1_setup_retry_no++;
      logger.info("Received F1SetupFailure with Time to Wait IE. Reinitiating F1 setup in {}s (retry={}/{}).",
                  failure->time_to_wait.value.to_number(),
                  f1_setup_retry_no,
                  request.setup_params.max_setup_retries);

      // TODO add timer

      CORO_AWAIT_VALUE(res, handle_f1ap_setup_request(request));
    } else {
      std::string cause_str = "";
      switch (failure->cause.value.type()) {
        case asn1::f1ap::cause_c::types_opts::radio_network:
          cause_str = failure->cause.value.radio_network().to_string();
          break;
        case asn1::f1ap::cause_c::types_opts::transport:
          cause_str = failure->cause.value.transport().to_string();
          break;
        case asn1::f1ap::cause_c::types_opts::protocol:
          cause_str = failure->cause.value.protocol().to_string();
          break;
        case asn1::f1ap::cause_c::types_opts::misc:
          cause_str = failure->cause.value.misc().to_string();
          break;
        default:
          cause_str = "unknown";
          break;
      }

      logger.error("Received F1SetupFailure with {} error: {}", failure->cause.value.type().to_string(), cause_str);
    }

    CORO_RETURN(res);
  });
}

void f1ap_du_impl::handle_pdu(f1_rx_pdu pdu)
{
  log_ue_event(logger, ue_event_prefix{"UL", pdu.ue_index}.set_channel("SRB0"), "Received PDU.");

  // FIXME: fill message
  asn1::f1ap::f1_ap_pdu_c f1ap_pdu;

  // Make it an initial message to let test pass.
  f1ap_pdu.set_init_msg();

  // send to handler
  f1c.handle_message(f1ap_pdu);
}

void f1ap_du_impl::handle_message(const asn1::f1ap::f1_ap_pdu_c& pdu)
{
  logger.info("Handling F1AP PDU of type {}", pdu.type().to_string());
  pdu_queue.try_push(pdu);
}

} // namespace srsgnb
