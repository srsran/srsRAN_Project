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

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between RRC UE and F1AP
class rrc_ue_f1ap_adapter : public rrc_pdu_notifier
{
public:
  explicit rrc_ue_f1ap_adapter(f1ap_rrc_message_handler& f1c_handler_, ue_index_t ue_index_) :
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
  f1ap_rrc_message_handler& f1c_handler;
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
  rrc_to_du_ue_task_scheduler(ue_index_t ue_index_, du_processor_ue_task_scheduler& du_processor_task_sched_) :
    ue_index(ue_index_), du_processor_task_sched(du_processor_task_sched_)
  {
  }

  void schedule_async_task(async_task<void>&& task) override
  {
    du_processor_task_sched.handle_ue_async_task(ue_index, std::move(task));
  }

  unique_timer make_unique_timer() override { return du_processor_task_sched.make_unique_timer(); }

private:
  ue_index_t                      ue_index;
  du_processor_ue_task_scheduler& du_processor_task_sched;
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

} // namespace srs_cu_cp
} // namespace srsgnb
