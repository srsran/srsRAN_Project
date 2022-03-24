
#include "f1ap_du_impl.h"
#include "../ran/gnb_format.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

f1ap_du_impl::f1ap_du_impl(f1ap_du_pdu_notifier& pdu_listener_, f1_du_gateway& gw_) :
  logger(srslog::fetch_basic_logger("F1AP")), pdu_listener(pdu_listener_), f1_gw(gw_)
{}

async_task<du_setup_result> f1ap_du_impl::f1ap_du_setup_request(const du_setup_params& params)
{
  // TODO: add procedure implementation
  return launch_async([this, res = du_setup_result{}](coro_context<async_task<du_setup_result> >& ctx) mutable {
    CORO_BEGIN(ctx);
    res.result = true;
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
  log_ul_pdu(logger, pdu.ue_index, INVALID_RNTI, MAX_NOF_CELLS, "SRB0", "CCCH Message");
  f1_gw.on_new_sdu(std::move(pdu.pdu));
}

void f1ap_du_impl::push_sdu(const byte_buffer& sdu) {}

} // namespace srsgnb
