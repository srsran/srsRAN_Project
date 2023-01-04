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

#include "../task_schedulers/ue_task_scheduler.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/du_processor.h"
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
    fill_supported_algorithms(sec_ctxt.supported_int_algos, caps.nr_integrity_protection_algorithms);
    fill_supported_algorithms(sec_ctxt.supported_enc_algos, caps.nr_encryption_algorithms);
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

/// Adapter between NGC and DU Processor
class ngc_du_processor_adapter : public ngc_du_processor_control_notifier
{
public:
  void connect_du_processor(du_processor_ngap_interface* du_processor_pdu_session_handler_)
  {
    du_processor_pdu_session_handler = du_processor_pdu_session_handler_;
  }

  async_task<cu_cp_pdu_session_resource_setup_response_message>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_message& request) override
  {
    srsgnb_assert(du_processor_pdu_session_handler != nullptr, "du_processor_pdu_session_handler must not be nullptr");

    return du_processor_pdu_session_handler->handle_new_pdu_session_resource_setup_request(request);
  }

private:
  du_processor_ngap_interface* du_processor_pdu_session_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
