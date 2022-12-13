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

#include "../helpers/cu_cp_asn1_helpers.h"
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

  async_task<pdu_session_resource_setup_response_message>
  on_new_pdu_session_resource_setup_request(pdu_session_resource_setup_message& msg) override
  {
    srsgnb_assert(e1_bearer_context_mng != nullptr, "e1_bearer_context_mng must not be nullptr");

    e1ap_bearer_context_setup_request_message e1_request;

    e1_request.sys_bearer_context_setup_request.set_ng_ran_bearer_context_setup_request();
    auto& ng_ran_bearer_context_setup_request =
        e1_request.sys_bearer_context_setup_request.ng_ran_bearer_context_setup_request();

    for (auto ngap_pdu_session_res_item : msg.pdu_session_res_setup_items) {
      asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_setup_request_o> bearer_request_item;
      auto& e1ap_pdu_session_res_items = bearer_request_item.value().pdu_session_res_to_setup_list();

      asn1::e1ap::pdu_session_res_to_setup_item_s e1ap_pdu_session_res_item;

      // pdu session id
      e1ap_pdu_session_res_item.pdu_session_id = ngap_pdu_session_res_item.pdu_session_id;

      // s-NSSAI
      e1ap_pdu_session_res_item.snssai.sst.from_number(ngap_pdu_session_res_item.s_nssai.sst.to_number());
      if (ngap_pdu_session_res_item.s_nssai.sd_present) {
        e1ap_pdu_session_res_item.snssai.sd_present = true;
        e1ap_pdu_session_res_item.snssai.sd.from_number(ngap_pdu_session_res_item.s_nssai.sd.to_number());
      }

      asn1::cbit_ref bref(ngap_pdu_session_res_item.pdu_session_res_setup_request_transfer);
      asn1::ngap::pdu_session_res_setup_request_transfer_s pdu_session_res_setup_request_transfer;
      // TODO: Check return value
      pdu_session_res_setup_request_transfer.unpack(bref);

      // pdu session type
      e1ap_pdu_session_res_item.pdu_session_type = ngap_pdu_session_type_to_e1ap_pdu_session_type(
          pdu_session_res_setup_request_transfer->pdu_session_type.value);

      // ng ul up transport layer information
      if (pdu_session_res_setup_request_transfer->ul_ngu_up_tnl_info->type() ==
          asn1::ngap::up_transport_layer_info_c::types::gtp_tunnel) {
        e1ap_pdu_session_res_item.ng_ul_up_tnl_info.set_gtp_tunnel();
        auto& e1ap_gtp_tunnel    = e1ap_pdu_session_res_item.ng_ul_up_tnl_info.gtp_tunnel();
        e1ap_gtp_tunnel.gtp_teid = pdu_session_res_setup_request_transfer->ul_ngu_up_tnl_info->gtp_tunnel().gtp_teid;
        e1ap_gtp_tunnel.transport_layer_address.from_number(
            pdu_session_res_setup_request_transfer->ul_ngu_up_tnl_info->gtp_tunnel()
                .transport_layer_address.to_number());
      } else {
        // TODO: error?
      }

      // TODO: add optional values

      // pdu session resource dl aggregate maximum bit rate
      if (pdu_session_res_setup_request_transfer->pdu_session_aggregate_maximum_bit_rate_present) {
        e1ap_pdu_session_res_item.pdu_session_res_dl_ambr_present = true;
        e1ap_pdu_session_res_item.pdu_session_res_dl_ambr =
            pdu_session_res_setup_request_transfer->pdu_session_aggregate_maximum_bit_rate
                ->pdu_session_aggregate_maximum_bit_rate_dl;
      }

      e1ap_pdu_session_res_items.push_back(e1ap_pdu_session_res_item);

      ng_ran_bearer_context_setup_request.push_back(bearer_request_item);
    }

    e1_request.uedl_aggregate_maximum_bit_rate = msg.ue_aggregate_maximum_bit_rate_dl;

    e1ap_bearer_context_setup_response_message e1_bearer_context_setup_resp_msg;

    return launch_async(
        [this, res = pdu_session_resource_setup_response_message{}, e1_bearer_context_setup_resp_msg, e1_request, msg](
            coro_context<async_task<pdu_session_resource_setup_response_message>>& ctx) mutable {
          CORO_BEGIN(ctx);

          CORO_AWAIT_VALUE(e1_bearer_context_setup_resp_msg,
                           e1_bearer_context_mng->handle_bearer_context_setup_request(e1_request));

          // Fail if E-UTRAN bearer context setup is returned
          if (e1_bearer_context_setup_resp_msg.response->sys_bearer_context_setup_resp->type() ==
              asn1::e1ap::sys_bearer_context_setup_resp_c::types::e_utran_bearer_context_setup_resp) {
            asn1::ngap::pdu_session_res_setup_unsuccessful_transfer_s setup_unsuccessful_transfer;
            setup_unsuccessful_transfer.cause.set_protocol();

            // Pack pdu_session_res_setup_unsuccessful_transfer_s
            byte_buffer   pdu;
            asn1::bit_ref bref(pdu);
            setup_unsuccessful_transfer.pack(bref);

            for (auto e1ap_failed_item : msg.pdu_session_res_setup_items) {
              asn1::ngap::pdu_session_res_failed_to_setup_item_su_res_s ngap_failed_item;
              ngap_failed_item.pdu_session_id                              = e1ap_failed_item.pdu_session_id;
              ngap_failed_item.pdu_session_res_setup_unsuccessful_transfer = std::move(pdu);
              res.pdu_session_res_failed_to_setup_items.push_back(ngap_failed_item);
            }

            CORO_EARLY_RETURN(res);
          }

          // Convert E1 response to common type
          fill_cu_cp_pdu_session_res_setup_response(res, msg, e1_bearer_context_setup_resp_msg);

          CORO_RETURN(res);
        });
  }

private:
  e1_bearer_context_manager* e1_bearer_context_mng = nullptr;
};

/// Adapter between NGC and F1C
class ngc_f1c_adapter : public ngc_f1c_control_notifier
{
public:
  void connect_f1c(f1c_ue_context_manager* f1c_ue_context_mng_) { f1c_ue_context_mng = f1c_ue_context_mng_; }

  async_task<pdu_session_resource_setup_response_message>
  on_new_pdu_session_resource_setup_request(pdu_session_resource_setup_message& msg) override
  {
    srsgnb_assert(f1c_ue_context_mng != nullptr, "f1c_ue_context_mng must not be nullptr");

    return launch_async([res = pdu_session_resource_setup_response_message{}](
                            coro_context<async_task<pdu_session_resource_setup_response_message>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(res);
    });
  }

private:
  f1c_ue_context_manager* f1c_ue_context_mng = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
