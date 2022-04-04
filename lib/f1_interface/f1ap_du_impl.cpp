
#include "f1ap_du_impl.h"
#include "../ran/gnb_format.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

f1ap_du_impl::f1ap_du_impl(f1c_pdu_handler& f1c_pdu_handler_) :
  logger(srslog::fetch_basic_logger("F1AP")), f1c(f1c_pdu_handler_)
{
}

async_task<du_setup_result> f1ap_du_impl::f1ap_du_setup_request(const du_setup_params& params)
{
  // TODO: add procedure implementation
  return launch_async([this, res = du_setup_result{}](coro_context<async_task<du_setup_result> >& ctx) mutable {
    CORO_BEGIN(ctx);
    res.result.value()->gnb_cu_rrc_version.value.latest_rrc_version.from_number(0);
    logger.info("{}", __FUNCTION__);
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
      ctxt.ue_ctxt_manager[msg.ue_index].gnb_cu_f1ap_ue_id = -1; // TODO

      resp.result = true;
    }
    CORO_RETURN(resp);
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
  f1c.handle_unpacked_pdu(f1ap_pdu);
}

void f1ap_du_impl::handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu)
{
  logger.info("Handling F1AP PDU of type {}", pdu.type().to_string());
}

} // namespace srsgnb
