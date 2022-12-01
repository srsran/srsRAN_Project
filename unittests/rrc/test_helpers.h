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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/rrc/rrc_ue.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_cu_cp {

class dummy_tx_pdu_handler
{
public:
  dummy_tx_pdu_handler() = default;
  void handle_pdu(byte_buffer_slice pdu) { last_pdu = std::move(pdu); }

  byte_buffer_slice last_pdu;
};

class dummy_rrc_pdu_notifier : public rrc_pdu_notifier
{
public:
  dummy_rrc_pdu_notifier(dummy_tx_pdu_handler& handler_) : handler(handler_) {}

  void on_new_pdu(const rrc_pdu_message& msg) override { handler.handle_pdu(byte_buffer_slice{std::move(msg.pdu)}); }

private:
  dummy_tx_pdu_handler& handler;
};

class dummy_du_processor_rrc_ue_interface : public du_processor_rrc_ue_interface
{
public:
  dummy_du_processor_rrc_ue_interface(ue_context& ue_ctxt_) : ue_ctxt(ue_ctxt_){};

  void create_srb(const srb_creation_message& msg) override
  {
    // create SRB1 or SRB2 PDU handler
    std::unique_ptr<dummy_rrc_pdu_notifier> tx_pdu_handler = nullptr;
    if (msg.srb_id == srb_id_t::srb1) {
      tx_pdu_handler = std::make_unique<dummy_rrc_pdu_notifier>(srb1_tx_pdu_handler);
      srb1_created   = true;
    } else if (msg.srb_id == srb_id_t::srb2) {
      tx_pdu_handler = std::make_unique<dummy_rrc_pdu_notifier>(srb2_tx_pdu_handler);
      srb2_created   = true;
    } else {
      return;
    }

    // connect SRB with RRC to "PDCP" adapter
    ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].rrc_tx_notifier = std::move(tx_pdu_handler);
    ue_ctxt.rrc->connect_srb_notifier(msg.srb_id, *ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].rrc_tx_notifier);
    last_srb = msg;
  }

  void handle_ue_context_release_command(const ue_context_release_command_message& msg) override
  {
    last_ue_ctxt_rel_cmd = msg;
  }

  ue_context_release_command_message last_ue_ctxt_rel_cmd;
  srb_creation_message               last_srb;
  dummy_tx_pdu_handler               srb0_tx_pdu_handler; // Object to handle the generated RRC message
  dummy_tx_pdu_handler               srb1_tx_pdu_handler; // Object to handle the generated RRC message
  dummy_tx_pdu_handler               srb2_tx_pdu_handler; // Object to handle the generated RRC message
  bool                               srb1_created = false;
  bool                               srb2_created = false;

private:
  ue_context& ue_ctxt;
};

class dummy_rrc_ue_du_processor_adapter : public rrc_ue_du_processor_notifier
{
public:
  void connect(dummy_du_processor_rrc_ue_interface& du_processor_rrc_ue_)
  {
    du_processor_rrc_ue_handler = &du_processor_rrc_ue_;
  }

  void on_create_srb(const srb_creation_message& msg) override { du_processor_rrc_ue_handler->create_srb(msg); }

  void on_ue_context_release_command(const ue_context_release_command_message& msg) override
  {
    du_processor_rrc_ue_handler->handle_ue_context_release_command(msg);
  }

private:
  dummy_du_processor_rrc_ue_interface* du_processor_rrc_ue_handler = nullptr;
};

class dummy_rrc_ue_ngc_adapter : public rrc_ue_nas_notifier, public rrc_ue_control_notifier
{
public:
  void on_initial_ue_message(const initial_ue_message& msg) override
  {
    logger.info("Received Initial UE Message");
    initial_ue_msg_received = true;
  }

  void on_ul_nas_transport_message(const ul_nas_transport_message& msg) override
  {
    logger.info("Received UL NAS Transport message");
  }

  void on_rrc_reconfiguration_complete(const rrc_reconfiguration_response_message& msg) override
  {
    logger.info("Received RRC Reconfiguration Response message");
  }

  bool initial_ue_msg_received = false;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

struct dummy_ue_task_scheduler : public rrc_ue_task_scheduler {
public:
  dummy_ue_task_scheduler(timer_manager& timers_) : timer_db(timers_) {}
  void           schedule_async_task(async_task<void>&& task) override { ctrl_loop.schedule(std::move(task)); }
  unique_timer   make_unique_timer() override { return timer_db.create_unique_timer(); }
  timer_manager& get_timer_manager() override { return timer_db; }

  void tick_timer() { timer_db.tick_all(); }

private:
  async_task_sequencer ctrl_loop{16};
  timer_manager&       timer_db;
};

} // namespace srs_cu_cp
} // namespace srsgnb
