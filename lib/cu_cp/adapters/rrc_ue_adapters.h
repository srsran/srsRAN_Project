/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../task_schedulers/ue_task_scheduler.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between RRC UE and F1AP to pass RRC PDUs
class rrc_ue_f1ap_pdu_adapter : public rrc_pdu_notifier
{
public:
  explicit rrc_ue_f1ap_pdu_adapter(f1ap_rrc_message_handler& f1ap_handler_, ue_index_t ue_index_) :
    f1ap_handler(f1ap_handler_), ue_index(ue_index_)
  {
  }

  void on_new_pdu(const rrc_pdu_message& msg) override
  {
    f1ap_dl_rrc_message f1ap_msg = {};
    f1ap_msg.ue_index            = ue_index;
    f1ap_msg.srb_id              = srb_id_t::srb0;
    f1ap_msg.rrc_container.resize(msg.pdu.length());
    std::copy(msg.pdu.begin(), msg.pdu.end(), f1ap_msg.rrc_container.begin());
    f1ap_handler.handle_dl_rrc_message_transfer(f1ap_msg);
  }

private:
  f1ap_rrc_message_handler& f1ap_handler;
  const ue_index_t          ue_index;
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

  void on_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) override
  {
    du_processor_rrc_ue_handler->handle_ue_context_release_command(cmd);
  }

private:
  du_processor_rrc_ue_interface* du_processor_rrc_ue_handler = nullptr;
};

/// Adapter between RRC UE and UE Task Scheduler
class rrc_to_du_ue_task_scheduler : public rrc_ue_task_scheduler
{
public:
  rrc_to_du_ue_task_scheduler(ue_index_t ue_index_) : ue_index(ue_index_) {}

  void connect_du_processor(du_processor_ue_task_handler& du_processor_task_handler_)
  {
    du_processor_task_handler = &du_processor_task_handler_;
  }

  void schedule_async_task(async_task<void>&& task) override
  {
    srsgnb_assert(du_processor_task_handler != nullptr, "DU Processor task handler must not be nullptr");
    du_processor_task_handler->handle_ue_async_task(ue_index, std::move(task));
  }

  unique_timer make_unique_timer() override
  {
    srsgnb_assert(du_processor_task_handler != nullptr, "DU Processor task handler must not be nullptr");
    return du_processor_task_handler->make_unique_timer();
  }
  timer_manager& get_timer_manager() override
  {
    srsgnb_assert(du_processor_task_handler != nullptr, "DU Processor task handler must not be nullptr");
    return du_processor_task_handler->get_timer_manager();
  }

private:
  ue_index_t                    ue_index;
  du_processor_ue_task_handler* du_processor_task_handler = nullptr;
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
  void enable_security(security::sec_128_as_config sec_cfg) override { pdcp_handler.enable_security(sec_cfg); }

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
  void enable_security(security::sec_128_as_config sec_cfg) override { pdcp_handler.enable_security(sec_cfg); }

private:
  pdcp_rx_upper_control_interface& pdcp_handler;
};

// Adapter between RRC UE and NGAP
class rrc_ue_ngap_adapter : public rrc_ue_nas_notifier, public rrc_ue_control_notifier
{
public:
  void connect_ngap(ngap_nas_message_handler& ngap_nas_msg_handler_) { ngap_nas_msg_handler = &ngap_nas_msg_handler_; }

  void on_initial_ue_message(const initial_ue_message& msg) override
  {
    srsgnb_assert(ngap_nas_msg_handler != nullptr, "NGAP handler must not be nullptr");

    ngap_initial_ue_message ngap_init_ue_msg;
    ngap_init_ue_msg.ue_index = msg.ue_index;
    ngap_init_ue_msg.nas_pdu  = msg.nas_pdu.copy();

    ngap_init_ue_msg.establishment_cause.value =
        rrc_establishment_cause_to_ngap_rrcestablishment_cause(msg.establishment_cause).value;

    ngap_init_ue_msg.nr_cgi.nr_cell_id.from_number(msg.cell.cgi.nci.packed);
    ngap_init_ue_msg.nr_cgi.plmn_id.from_string(msg.cell.cgi.plmn_hex);
    ngap_init_ue_msg.tac = msg.cell.tac;

    ngap_nas_msg_handler->handle_initial_ue_message(ngap_init_ue_msg);
  }

  void on_ul_nas_transport_message(const ul_nas_transport_message& msg) override
  {
    srsgnb_assert(ngap_nas_msg_handler != nullptr, "NGAP handler must not be nullptr");

    ngap_ul_nas_transport_message ngap_ul_nas_msg;
    ngap_ul_nas_msg.ue_index = msg.ue_index;
    ngap_ul_nas_msg.nas_pdu  = msg.nas_pdu.copy();

    ngap_ul_nas_msg.nr_cgi.nr_cell_id.from_number(msg.cell.cgi.nci.packed);
    ngap_ul_nas_msg.nr_cgi.plmn_id.from_string(msg.cell.cgi.plmn_hex);
    ngap_ul_nas_msg.tac = msg.cell.tac;

    ngap_nas_msg_handler->handle_ul_nas_transport_message(ngap_ul_nas_msg);
  }

private:
  ngap_nas_message_handler* ngap_nas_msg_handler = nullptr;

  /// \brief Convert a RRC Establishment Cause to a NGAP RRC Establishment Cause.
  /// \param establishment_cause The RRC Establishment Cause.
  /// \return The NGAP RRC Establishment Cause.
  inline asn1::ngap::rrc_establishment_cause_opts rrc_establishment_cause_to_ngap_rrcestablishment_cause(
      const asn1::rrc_nr::establishment_cause_opts& establishment_cause)
  {
    asn1::ngap::rrc_establishment_cause_opts rrcestablishment_cause = {};
    switch (establishment_cause.value) {
      case asn1::rrc_nr::establishment_cause_opts::options::emergency:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::emergency;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::high_prio_access:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::high_prio_access;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mt_access:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mt_access;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_sig:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mo_sig;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_data:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mo_data;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_voice_call:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mo_voice_call;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_video_call:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mo_video_call;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mo_sms:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mo_sms;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mps_prio_access:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mps_prio_access;
        break;
      case asn1::rrc_nr::establishment_cause_opts::options::mcs_prio_access:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::mcs_prio_access;
        break;
      default:
        rrcestablishment_cause.value = asn1::ngap::rrc_establishment_cause_opts::nulltype;
        break;
    }

    return rrcestablishment_cause;
  }
};

} // namespace srs_cu_cp
} // namespace srsran
