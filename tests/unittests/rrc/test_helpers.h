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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/rrc/rrc_du.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsran {
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
  void enable_security(security::sec_128_as_config sec_cfg) override
  {
    integ_enabled  = true;
    cipher_enabled = true;
    sec_configured = true;
    last_sec_cfg   = sec_cfg;
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
  void enable_security(security::sec_128_as_config sec_cfg) override
  {
    integ_enabled  = true;
    cipher_enabled = true;
    sec_configured = true;
    last_sec_cfg   = sec_cfg;
  }

  security::sec_128_as_config last_sec_cfg   = {};
  bool                        sec_configured = false;
  bool                        integ_enabled  = false;
  bool                        cipher_enabled = false;
};

class dummy_rrc_ue_du_processor_adapter : public rrc_ue_du_processor_notifier
{
public:
  void on_create_srb(const srb_creation_message& msg) override
  {
    logger.info("Received SRB creation message");
    if (msg.srb_id == srb_id_t::srb1) {
      srb1_created = true;
    } else if (msg.srb_id == srb_id_t::srb2) {
      srb2_created = true;
    }
    last_srb_creation_message = std::move(msg);
  }

  void on_ue_context_release_command(const cu_cp_ue_context_release_command& msg) override
  {
    logger.info("Received UE Context Release Command");
    last_cu_cp_ue_context_release_command = msg;
  }

  srb_creation_message             last_srb_creation_message;
  bool                             srb1_created = false;
  bool                             srb2_created = false;
  cu_cp_ue_context_release_command last_cu_cp_ue_context_release_command;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

class dummy_rrc_ue_ngap_adapter : public rrc_ue_nas_notifier, public rrc_ue_control_notifier
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
} // namespace srsran
