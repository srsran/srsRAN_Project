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

#include "../helpers/ngap_e1ap_asn1_helpers.h"
#include "../task_schedulers/ue_task_scheduler.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/ran/bcd_helpers.h"
#include "srsgnb/rrc/rrc_ue.h"
#include "srsgnb/srslog/srslog.h"

#include "../../ngap/ngap_asn1_utils.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between NGC and Task Scheduler
class ngc_to_cu_cp_task_scheduler : public ngc_ue_task_scheduler
{
public:
  ngc_to_cu_cp_task_scheduler() {}

  void connect_cu_cp(ue_task_scheduler& cu_cp_task_sched_) { cu_cp_task_sched = &cu_cp_task_sched_; }

  void schedule_async_task(cu_cp_ue_id_t cu_cp_ue_id, async_task<void>&& task) override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    cu_cp_task_sched->handle_ue_async_task(cu_cp_ue_id, std::move(task));
  }

  unique_timer make_unique_timer() override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->make_unique_timer();
  }
  timer_manager& get_timer_manager() override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->get_timer_manager();
  }

private:
  ue_task_scheduler* cu_cp_task_sched = nullptr;
};

/// Adapter between NGC and RRC UE
class ngc_rrc_ue_adapter : public ngc_rrc_ue_pdu_notifier, public ngc_rrc_ue_control_notifier
{
public:
  void connect_rrc_ue(rrc_ue_dl_nas_message_handler*        rrc_ue_msg_handler_,
                      rrc_ue_control_message_handler*       rrc_ue_ctrl_handler_,
                      rrc_ue_init_security_context_handler* rrc_ue_security_handler_)
  {
    rrc_ue_msg_handler      = rrc_ue_msg_handler_;
    rrc_ue_ctrl_handler     = rrc_ue_ctrl_handler_;
    rrc_ue_security_handler = rrc_ue_security_handler_;
  }

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    srsgnb_assert(rrc_ue_msg_handler != nullptr, "rrc_ue_msg_handler must not be nullptr");

    dl_nas_transport_message dl_nas_msg = {};
    dl_nas_msg.nas_pdu                  = std::move(nas_pdu);

    rrc_ue_msg_handler->handle_dl_nas_transport_message(dl_nas_msg);
  }

  async_task<bool> on_new_security_context(const asn1::ngap::ue_security_cap_s&           caps,
                                           const asn1::fixed_bitstring<256, false, true>& key) override
  {
    srsgnb_assert(rrc_ue_security_handler != nullptr, "rrc_ue_security_handler must not be nullptr");

    rrc_init_security_context sec_ctxt;
    copy_asn1_key(sec_ctxt.k, key);
    fill_supported_algorithms(sec_ctxt.supported_int_algos, caps.nrintegrity_protection_algorithms);
    fill_supported_algorithms(sec_ctxt.supported_enc_algos, caps.nrencryption_algorithms);
    logger.debug(key.data(), 32, "K_gnb");
    logger.debug("Supported integrity algorithms: {}", sec_ctxt.supported_int_algos);
    logger.debug("Supported ciphering algorithms: {}", sec_ctxt.supported_enc_algos);

    return rrc_ue_security_handler->handle_init_security_context(sec_ctxt);
  }

private:
  rrc_ue_dl_nas_message_handler*        rrc_ue_msg_handler      = nullptr;
  rrc_ue_control_message_handler*       rrc_ue_ctrl_handler     = nullptr;
  rrc_ue_init_security_context_handler* rrc_ue_security_handler = nullptr;
  srslog::basic_logger&                 logger                  = srslog::fetch_basic_logger("NGC");
};

/// Adapter between NGC and E1
class ngc_e1_adapter : public ngc_e1_control_notifier
{
public:
  void connect_e1(e1_bearer_context_manager* e1_bearer_context_mng_) { e1_bearer_context_mng = e1_bearer_context_mng_; }

  async_task<e1ap_pdu_session_resource_setup_response_message>
  on_new_pdu_session_resource_setup_request(e1ap_pdu_session_resource_setup_message& msg) override
  {
    srsgnb_assert(e1_bearer_context_mng != nullptr, "e1_bearer_context_mng must not be nullptr");

    e1ap_bearer_context_setup_request_message e1_request;
    fill_e1ap_bearer_context_setup_request(e1_request, msg);

    e1ap_bearer_context_setup_response_message e1_bearer_context_setup_resp_msg;

    return launch_async([this,
                         res = e1ap_pdu_session_resource_setup_response_message{},
                         e1_bearer_context_setup_resp_msg,
                         e1_request,
                         msg](coro_context<async_task<e1ap_pdu_session_resource_setup_response_message>>& ctx) mutable {
      CORO_BEGIN(ctx);

      CORO_AWAIT_VALUE(e1_bearer_context_setup_resp_msg,
                       e1_bearer_context_mng->handle_bearer_context_setup_request(e1_request));

      // Fail if E-UTRAN bearer context setup is returned
      if (e1_bearer_context_setup_resp_msg.response->sys_bearer_context_setup_resp->type() ==
          asn1::e1ap::sys_bearer_context_setup_resp_c::types::e_utran_bearer_context_setup_resp) {
        asn1::ngap::cause_c cause;
        cause.set(asn1::ngap::cause_c::types_opts::options::protocol);
        fill_failed_ngap_pdu_session_res_setup_response(res, msg, e1_bearer_context_setup_resp_msg, cause);

        CORO_EARLY_RETURN(res);
      }

      // TODO: Fill all values of the response
      fill_ngap_pdu_session_res_setup_response(res, msg, e1_bearer_context_setup_resp_msg);

      CORO_RETURN(res);
    });
  }

private:
  e1_bearer_context_manager* e1_bearer_context_mng = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
