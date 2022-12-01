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

class dummy_rrc_pdu_notifier : public rrc_pdu_notifier
{
public:
  dummy_rrc_pdu_notifier() = default;

  void on_new_pdu(const rrc_pdu_message& msg) override { last_pdu = byte_buffer_slice{msg.pdu}; }

  byte_buffer_slice last_pdu;
};

class dummy_rrc_tx_security_notifier : public rrc_tx_security_notifier
{
public:
  explicit dummy_rrc_tx_security_notifier() = default;
  void set_as_security_config(security::sec_128_as_config sec_cfg) override
  {
    sec_configured = true;
    last_sec_cfg   = sec_cfg;
  }
  void enable_or_disable_security(security::integrity_enabled integ, security::ciphering_enabled cipher) override
  {
    integ_enabled  = integ == security::integrity_enabled::enabled;
    cipher_enabled = cipher == security::ciphering_enabled::enabled;
  }

  security::sec_128_as_config last_sec_cfg   = {};
  bool                        sec_configured = false;
  bool                        integ_enabled  = false;
  bool                        cipher_enabled = false;
};

class dummy_rrc_rx_security_notifier : public rrc_rx_security_notifier
{
public:
  explicit dummy_rrc_rx_security_notifier() {}
  void set_as_security_config(security::sec_128_as_config sec_cfg) override
  {
    sec_configured = true;
    last_sec_cfg   = sec_cfg;
  }
  void enable_or_disable_security(security::integrity_enabled integ, security::ciphering_enabled cipher) override
  {
    integ_enabled  = integ == security::integrity_enabled::enabled;
    cipher_enabled = cipher == security::ciphering_enabled::enabled;
  }

  security::sec_128_as_config last_sec_cfg   = {};
  bool                        sec_configured = false;
  bool                        integ_enabled  = false;
  bool                        cipher_enabled = false;
};

class dummy_du_processor_rrc_ue_interface : public du_processor_rrc_ue_interface
{
public:
  dummy_du_processor_rrc_ue_interface(ue_context& ue_ctxt_) : ue_ctxt(ue_ctxt_){};

  void create_srb(const srb_creation_message& msg) override
  {
    // create SRB1 or SRB2 PDU handler
    std::unique_ptr<dummy_rrc_pdu_notifier>         tx_pdu_notifier      = nullptr;
    std::unique_ptr<dummy_rrc_tx_security_notifier> tx_security_notifier = nullptr;
    std::unique_ptr<dummy_rrc_rx_security_notifier> rx_security_notifier = nullptr;
    if (msg.srb_id == srb_id_t::srb1) {
      tx_pdu_notifier          = std::make_unique<dummy_rrc_pdu_notifier>();
      tx_security_notifier     = std::make_unique<dummy_rrc_tx_security_notifier>();
      rx_security_notifier     = std::make_unique<dummy_rrc_rx_security_notifier>();
      srb1_tx_pdu_handler      = tx_pdu_notifier.get();
      srb1_tx_security_handler = tx_security_notifier.get();
      srb1_rx_security_handler = rx_security_notifier.get();
      srb1_created             = true;
    } else if (msg.srb_id == srb_id_t::srb2) {
      tx_pdu_notifier          = std::make_unique<dummy_rrc_pdu_notifier>();
      tx_security_notifier     = std::make_unique<dummy_rrc_tx_security_notifier>();
      rx_security_notifier     = std::make_unique<dummy_rrc_rx_security_notifier>();
      srb2_tx_pdu_handler      = tx_pdu_notifier.get();
      srb2_tx_security_handler = tx_security_notifier.get();
      srb2_rx_security_handler = rx_security_notifier.get();
      srb2_created             = true;
    } else {
      return;
    }

    // create PDCP context for this SRB
    ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].pdcp_context.emplace();

    // connect SRB with RRC to "PDCP" adapter
    ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].rrc_tx_notifier                   = std::move(tx_pdu_notifier);
    ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].pdcp_context->rrc_tx_sec_notifier = std::move(tx_security_notifier);
    ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].pdcp_context->rrc_rx_sec_notifier = std::move(rx_security_notifier);
    ue_ctxt.rrc->connect_srb_notifier(msg.srb_id,
                                      *ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].rrc_tx_notifier,
                                      ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].pdcp_context->rrc_tx_sec_notifier.get(),
                                      ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].pdcp_context->rrc_rx_sec_notifier.get());
    last_srb = msg;
  }

  void handle_ue_context_release_command(const ue_context_release_command_message& msg) override
  {
    last_ue_ctxt_rel_cmd = msg;
  }

  ue_context_release_command_message last_ue_ctxt_rel_cmd;
  srb_creation_message               last_srb;
  dummy_rrc_pdu_notifier*            srb0_tx_pdu_handler = nullptr; // Object to handle the generated RRC message (SRB0)
  dummy_rrc_pdu_notifier*            srb1_tx_pdu_handler = nullptr; // Object to handle the generated RRC message (SRB1)
  dummy_rrc_pdu_notifier*            srb2_tx_pdu_handler = nullptr; // Object to handle the generated RRC message (SRB2)
  dummy_rrc_tx_security_notifier* srb1_tx_security_handler = nullptr; // Object to handle the tx security control (SRB2)
  dummy_rrc_rx_security_notifier* srb1_rx_security_handler = nullptr; // Object to handle the tx security control (SRB2)
  dummy_rrc_tx_security_notifier* srb2_tx_security_handler = nullptr; // Object to handle the tx security control (SRB2)
  dummy_rrc_rx_security_notifier* srb2_rx_security_handler = nullptr; // Object to handle the tx security control (SRB2)
  bool                            srb1_created             = false;
  bool                            srb2_created             = false;

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
