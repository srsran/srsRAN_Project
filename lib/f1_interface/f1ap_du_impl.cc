
#include "f1ap_du_impl.h"
#include "../ran/gnb_format.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

f1ap_du_impl::f1ap_du_impl(f1ap_du_pdu_notifier& pdu_listener_) :
  logger(srslog::fetch_basic_logger("F1AP")), pdu_listener(pdu_listener_)
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

async_task<f1ap_du_ue_create_response_message>
f1ap_du_impl::handle_ue_creation_request(const f1ap_du_ue_create_request_message& msg)
{
  // TODO: F1 procedure
  return launch_async([](coro_context<async_task<f1ap_du_ue_create_response_message> >& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(f1ap_du_ue_create_response_message{true});
  });
}

void f1ap_du_impl::handle_pdu(f1_rx_pdu pdu)
{
  log_ul_pdu(logger, pdu.ue_index, INVALID_RNTI, MAX_NOF_CELLS, "SRB0", "CCCH Message");
  pdu_listener.push_pdu(std::move(pdu.pdu));
}

void f1ap_du_impl::push_sdu(const byte_buffer& sdu) {}

} // namespace srsgnb
