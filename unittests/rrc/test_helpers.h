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
    // set notifier to a known value (i.e. nullptr) to be able to check if it was called
    ue_ctxt.srbs[srb_id_to_uint(msg.srb_id)].rrc_tx_notifier.reset();
    last_srb = msg;
  }

  void handle_ue_context_release_command(const ue_context_release_command_message& msg) override
  {
    last_ue_ctxt_rel_cmd = msg;
  }

  ue_context_release_command_message last_ue_ctxt_rel_cmd;
  srb_creation_message               last_srb;

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

class dummy_rrc_ue_ngc_adapter : public rrc_ue_nas_notifier
{
public:
  void on_initial_ue_message(const initial_ue_message& msg) override { logger.info("Received Initial UE Message"); }

  void on_ul_nas_transport_message(const ul_nas_transport_message& msg) override
  {
    logger.info("Received UL NAS Transport message");
  }

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

/// Reusable notifier class that a) stores the received msg for test inspection and b)
/// calls the registered msg handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_rrc_ue_nas_notifier : public srs_cu_cp::rrc_ue_nas_notifier
{
public:
  // FIXME: Add handler when ngc exists
  dummy_rrc_ue_nas_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_initial_ue_message(const srs_cu_cp::initial_ue_message& msg) override
  {
    logger.info("Received Initial UE message");
  };

  void on_ul_nas_transport_message(const srs_cu_cp::ul_nas_transport_message& msg) override
  {
    logger.info("Received UL NAS transport message");
  };

private:
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsgnb
