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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1c/cu_cp/f1c_cu.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between RRC UE and F1AP
class rrc_ue_f1ap_adapter : public rrc_pdu_notifier
{
public:
  explicit rrc_ue_f1ap_adapter(f1c_rrc_message_handler& f1c_handler_, ue_index_t ue_index_) :
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

/// Adapter between RRC and PDCP in DL direction (Tx)
class rrc_ue_pdcp_adapter : public rrc_pdu_notifier
{
public:
  explicit rrc_ue_pdcp_adapter(pdcp_tx_upper_data_interface& pdcp_handler_) : pdcp_handler(pdcp_handler_) {}

  void on_new_pdu(const rrc_pdu_message& msg) override { pdcp_handler.handle_sdu({msg.pdu.begin(), msg.pdu.end()}); }

private:
  pdcp_tx_upper_data_interface& pdcp_handler;
};

// Adapter between RRC UE and NGC
class rrc_ue_ngc_adapter : public rrc_ue_nas_notifier
{
public:
  void connect_ngc(ngc_nas_message_handler& ngc_nas_msg_handler_) { ngc_nas_msg_handler = &ngc_nas_msg_handler_; }

  void set_du_index(du_index_t du_index_) { du_index = du_index_; }

  void on_initial_ue_message(const initial_ue_message& msg) override
  {
    srsgnb_assert(du_index != INVALID_DU_INDEX, "du_index of rrc_ue_ngc_adapter not set");

    ngap_initial_ue_message ngap_init_ue_msg;
    ngap_init_ue_msg.ue_ngap_id = get_ue_ngap_id(du_index, msg.ue_index);
    ngap_init_ue_msg.nas_pdu.resize(msg.ded_nas_msg.size());
    std::copy(msg.ded_nas_msg.begin(), msg.ded_nas_msg.end(), ngap_init_ue_msg.nas_pdu.begin());

    ngap_init_ue_msg.establishment_cause.value =
        rrcestablishment_cause_from_establishment_cause(msg.establishment_cause).value;

    ngap_init_ue_msg.nr_cgi.nrcell_id.from_number(msg.cgi.nci.packed);
    ngap_init_ue_msg.nr_cgi.plmn_id.from_string(msg.cgi.plmn_hex);

    ngc_nas_msg_handler->handle_initial_ue_message(ngap_init_ue_msg);
  }

  void on_ul_nas_transport_message(const ul_nas_transport_message& msg) override
  {
    srsgnb_assert(du_index != INVALID_DU_INDEX, "du_index of rrc_ue_ngc_adapter not set");

    ngap_ul_nas_transport_message ngap_ul_nas_msg;
    ngap_ul_nas_msg.ue_ngap_id = get_ue_ngap_id(du_index, msg.ue_index);
    ngap_ul_nas_msg.nas_pdu.resize(msg.ded_nas_msg.size());
    std::copy(msg.ded_nas_msg.begin(), msg.ded_nas_msg.end(), ngap_ul_nas_msg.nas_pdu.begin());

    ngap_ul_nas_msg.nr_cgi.nrcell_id.from_number(msg.cgi.nci.packed);
    ngap_ul_nas_msg.nr_cgi.plmn_id.from_string(msg.cgi.plmn_hex);

    ngc_nas_msg_handler->handle_ul_nas_transport_message(ngap_ul_nas_msg);
  }

private:
  asn1::ngap::rrcestablishment_cause_opts
  rrcestablishment_cause_from_establishment_cause(const asn1::rrc_nr::establishment_cause_opts& establishment_cause)
  {
    asn1::ngap::rrcestablishment_cause_opts rrcestablishment_cause = {};
    switch (establishment_cause.value) {
      case asn1::rrc_nr::establishment_cause_opts::options::emergency:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::emergency;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::high_prio_access:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::high_prio_access;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mt_access:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mt_access;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_sig:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_sig;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_data:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_data;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_voice_call:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_voice_call;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_video_call:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_video_call;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_sms:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_sms;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mps_prio_access:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mps_prio_access;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mcs_prio_access:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mcs_prio_access;
        break;
      default:
        rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::nulltype;
        break;
    }

    return rrcestablishment_cause;
  }

  ngc_nas_message_handler* ngc_nas_msg_handler = nullptr;
  du_index_t               du_index            = INVALID_DU_INDEX;
};

} // namespace srs_cu_cp
} // namespace srsgnb
