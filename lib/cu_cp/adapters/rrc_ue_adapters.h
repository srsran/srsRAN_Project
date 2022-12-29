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

#include "../converters/rrc_ue_asn1_converters.h"
#include "../helpers/rrc_ue_f1ap_asn1_helpers.h"
#include "../task_schedulers/ue_task_scheduler.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/rrc/rrc_ue.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between RRC UE and F1AP to pass RRC PDUs
class rrc_ue_f1ap_pdu_adapter : public rrc_pdu_notifier
{
public:
  explicit rrc_ue_f1ap_pdu_adapter(f1c_rrc_message_handler& f1c_handler_, ue_index_t ue_index_) :
    f1c_handler(f1c_handler_), ue_index(ue_index_)
  {
  }

  void on_new_pdu(const rrc_pdu_message& msg) override
  {
    f1ap_dl_rrc_message f1ap_msg = {};
    f1ap_msg.ue_index            = ue_index;
    f1ap_msg.srb_id              = srb_id_t::srb0;
    f1ap_msg.rrc_container.resize(msg.pdu.length());
    std::copy(msg.pdu.begin(), msg.pdu.end(), f1ap_msg.rrc_container.begin());
    f1c_handler.handle_dl_rrc_message_transfer(f1ap_msg);
  }

private:
  f1c_rrc_message_handler& f1c_handler;
  const ue_index_t         ue_index;
};

/// Adapter between RRC UE and F1AP to trigger procedures
class rrc_ue_f1ap_control_adapter : public rrc_ue_f1c_control_notifier
{
public:
  explicit rrc_ue_f1ap_control_adapter(f1c_ue_context_manager& f1c_ue_context_mng_) :
    f1c_ue_context_mng(f1c_ue_context_mng_)
  {
  }

  async_task<rrc_ue_ue_context_modification_response_message>
  on_new_pdu_session_resource_setup_request(rrc_ue_ue_context_modification_request_message& msg) override
  {
    f1ap_ue_context_modification_request f1c_ue_ctxt_mod_req;
    fill_f1ap_ue_context_modification_request(f1c_ue_ctxt_mod_req, msg);

    f1ap_ue_context_modification_response f1c_ue_ctxt_mod_resp;

    return launch_async(
        [this, res = rrc_ue_ue_context_modification_response_message{}, f1c_ue_ctxt_mod_resp, f1c_ue_ctxt_mod_req, msg](
            coro_context<async_task<rrc_ue_ue_context_modification_response_message>>& ctx) mutable {
          CORO_BEGIN(ctx);

          CORO_AWAIT_VALUE(f1c_ue_ctxt_mod_resp,
                           f1c_ue_context_mng.handle_ue_context_modification_request(f1c_ue_ctxt_mod_req));

          fill_rrc_ue_ue_context_modification_response_message(res, f1c_ue_ctxt_mod_resp);

          CORO_RETURN(res);
        });
  }

private:
  f1c_ue_context_manager& f1c_ue_context_mng;
};

/// Adapter between RRC UE and E1
class rrc_ue_e1_adapter : public rrc_ue_e1_control_notifier
{
public:
  void connect_e1(e1_bearer_context_manager* e1_bearer_context_mng_) { e1_bearer_context_mng = e1_bearer_context_mng_; }

  async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override
  {
    srsgnb_assert(e1_bearer_context_mng != nullptr, "e1_bearer_context_mng must not be nullptr");

    return launch_async([this, res = e1ap_bearer_context_setup_response{}, msg](
                            coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      CORO_AWAIT_VALUE(res, e1_bearer_context_mng->handle_bearer_context_setup_request(msg));

      CORO_RETURN(res);
    });
  }

  async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override
  {
    srsgnb_assert(e1_bearer_context_mng != nullptr, "e1_bearer_context_mng must not be nullptr");

    return launch_async([this, res = e1ap_bearer_context_modification_response{}, msg](
                            coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      CORO_AWAIT_VALUE(res, e1_bearer_context_mng->handle_bearer_context_modification_request(msg));

      CORO_RETURN(res);
    });
  }

private:
  e1_bearer_context_manager* e1_bearer_context_mng = nullptr;
};

/// Adapter between RRC UE and DU processor
class rrc_ue_du_processor_adapter : public rrc_ue_du_processor_notifier
{
public:
  void connect_du_processor(du_processor_rrc_ue_interface& du_processor_rrc_ue_)
  {
    du_processor_rrc_ue_handler = &du_processor_rrc_ue_;
  }

  void on_create_srb(const srb_creation_message& msg) override { du_processor_rrc_ue_handler->create_srb(msg); }

  void on_ue_context_release_command(const ue_context_release_command_message& msg) override
  {
    du_processor_rrc_ue_handler->handle_ue_context_release_command(msg);
  }

private:
  du_processor_rrc_ue_interface* du_processor_rrc_ue_handler = nullptr;
};

/// Adapter between RRC UE and UE Task Scheduler
class rrc_to_du_ue_task_scheduler : public rrc_ue_task_scheduler
{
public:
  rrc_to_du_ue_task_scheduler(ue_index_t ue_index_, du_processor_ue_task_handler& du_processor_task_handler_) :
    ue_index(ue_index_), du_processor_task_handler(du_processor_task_handler_)
  {
  }

  void schedule_async_task(async_task<void>&& task) override
  {
    du_processor_task_handler.handle_ue_async_task(ue_index, std::move(task));
  }

  unique_timer   make_unique_timer() override { return du_processor_task_handler.make_unique_timer(); }
  timer_manager& get_timer_manager() override { return du_processor_task_handler.get_timer_manager(); }

private:
  ue_index_t                    ue_index;
  du_processor_ue_task_handler& du_processor_task_handler;
};

/// Adapter between RRC and PDCP in DL direction (Tx) for data transfer
class rrc_ue_pdcp_pdu_adapter : public rrc_pdu_notifier
{
public:
  explicit rrc_ue_pdcp_pdu_adapter(pdcp_tx_upper_data_interface& pdcp_handler_) : pdcp_handler(pdcp_handler_) {}

  void on_new_pdu(const rrc_pdu_message& msg) override { pdcp_handler.handle_sdu({msg.pdu.begin(), msg.pdu.end()}); }

private:
  pdcp_tx_upper_data_interface& pdcp_handler;
};

/// Adapter between RRC and PDCP in DL direction (Tx) for security configuration
class rrc_ue_pdcp_tx_security_adapter final : public rrc_tx_security_notifier
{
public:
  explicit rrc_ue_pdcp_tx_security_adapter(pdcp_tx_upper_control_interface& pdcp_handler_) : pdcp_handler(pdcp_handler_)
  {
  }

  // Setup security
  void set_as_security_config(security::sec_128_as_config sec_cfg) override
  {
    pdcp_handler.set_as_security_config(sec_cfg);
  }
  void enable_or_disable_security(security::integrity_enabled integ, security::ciphering_enabled cipher) override
  {
    pdcp_handler.enable_or_disable_security(integ, cipher);
  };

private:
  pdcp_tx_upper_control_interface& pdcp_handler;
};

/// Adapter between RRC and PDCP in DL direction (Rx) for security configuration
class rrc_ue_pdcp_rx_security_adapter final : public rrc_rx_security_notifier
{
public:
  explicit rrc_ue_pdcp_rx_security_adapter(pdcp_rx_upper_control_interface& pdcp_handler_) : pdcp_handler(pdcp_handler_)
  {
  }

  // Setup security
  void set_as_security_config(security::sec_128_as_config sec_cfg) override
  {
    pdcp_handler.set_as_security_config(sec_cfg);
  }
  void enable_or_disable_security(security::integrity_enabled integ, security::ciphering_enabled cipher) override
  {
    pdcp_handler.enable_or_disable_security(integ, cipher);
  };

private:
  pdcp_rx_upper_control_interface& pdcp_handler;
};

// Adapter between RRC UE and NGC
class rrc_ue_ngc_adapter : public rrc_ue_nas_notifier, public rrc_ue_control_notifier
{
public:
  void connect_ngc(ngc_nas_message_handler& ngc_nas_msg_handler_) { ngc_nas_msg_handler = &ngc_nas_msg_handler_; }

  void set_du_index(du_index_t du_index_) { du_index = du_index_; }

  void on_initial_ue_message(const initial_ue_message& msg) override
  {
    srsgnb_assert(du_index != INVALID_DU_INDEX, "du_index of rrc_ue_ngc_adapter not set");
    srsgnb_assert(ngc_nas_msg_handler != nullptr, "ngc_nas_msg_handler must not be nullptr");

    ngap_initial_ue_message ngap_init_ue_msg;
    ngap_init_ue_msg.cu_cp_ue_id = get_cu_cp_ue_id(du_index, msg.ue_index);
    ngap_init_ue_msg.nas_pdu     = msg.nas_pdu.copy();

    ngap_init_ue_msg.establishment_cause.value =
        rrc_establishment_cause_to_ngap_rrcestablishment_cause(msg.establishment_cause).value;

    ngap_init_ue_msg.nr_cgi.nr_cell_id.from_number(msg.cell.cgi.nci.packed);
    ngap_init_ue_msg.nr_cgi.plmn_id.from_string(msg.cell.cgi.plmn_hex);
    ngap_init_ue_msg.tac = msg.cell.tac;

    ngc_nas_msg_handler->handle_initial_ue_message(ngap_init_ue_msg);
  }

  void on_ul_nas_transport_message(const ul_nas_transport_message& msg) override
  {
    srsgnb_assert(du_index != INVALID_DU_INDEX, "du_index of rrc_ue_ngc_adapter not set");
    srsgnb_assert(ngc_nas_msg_handler != nullptr, "ngc_nas_msg_handler must not be nullptr");

    ngap_ul_nas_transport_message ngap_ul_nas_msg;
    ngap_ul_nas_msg.cu_cp_ue_id = get_cu_cp_ue_id(du_index, msg.ue_index);
    ngap_ul_nas_msg.nas_pdu     = msg.nas_pdu.copy();

    ngap_ul_nas_msg.nr_cgi.nr_cell_id.from_number(msg.cell.cgi.nci.packed);
    ngap_ul_nas_msg.nr_cgi.plmn_id.from_string(msg.cell.cgi.plmn_hex);
    ngap_ul_nas_msg.tac = msg.cell.tac;

    ngc_nas_msg_handler->handle_ul_nas_transport_message(ngap_ul_nas_msg);
  }

private:
  ngc_nas_message_handler* ngc_nas_msg_handler = nullptr;
  du_index_t               du_index            = INVALID_DU_INDEX;
};

} // namespace srs_cu_cp
} // namespace srsgnb
