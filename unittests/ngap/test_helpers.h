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
#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_cu_cp {

struct dummy_ngc_ue_task_scheduler : public ngc_ue_task_scheduler {
public:
  dummy_ngc_ue_task_scheduler(timer_manager& timers_) : timer_db(timers_) {}
  void schedule_async_task(cu_cp_ue_id_t cu_cp_ue_id, async_task<void>&& task) override
  {
    ctrl_loop.schedule(std::move(task));
  }
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
class dummy_ngc_amf_notifier : public ngc_message_notifier
{
public:
  dummy_ngc_amf_notifier(ngc_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(ngc_message_handler* handler_) { handler = handler_; };

  void on_new_message(const ngc_message& msg) override
  {
    logger.info("Received NGC message");
    last_ngc_msg = msg;

    if (handler != nullptr) {
      logger.info("Forwarding NGC PDU");
      handler->handle_message(msg);
    }
  };

  ngc_message last_ngc_msg;

private:
  srslog::basic_logger& logger;
  ngc_message_handler*  handler = nullptr;
};

/// Dummy handler storing and printing the received PDU.
class dummy_ngc_message_handler : public ngc_message_handler
{
public:
  dummy_ngc_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const ngc_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a NGAP PDU of type {}", msg.pdu.type().to_string());
  }
  ngc_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy NGC to RRC UE PDU notifier
class dummy_ngc_rrc_ue_notifier : public ngc_rrc_ue_pdu_notifier, public ngc_rrc_ue_control_notifier
{
public:
  dummy_ngc_rrc_ue_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    last_nas_pdu = std::move(nas_pdu);
    logger.info("Received a NAS PDU");
  }

  async_task<bool> on_new_security_context(const asn1::ngap::ue_security_cap_s&           caps,
                                           const asn1::fixed_bitstring<256, false, true>& key) override
  {
    logger.info("Received a new security context");
    return launch_async([this](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  byte_buffer last_nas_pdu;

private:
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsgnb