/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task_loop.h"

namespace srsran {
namespace srs_cu_cp {

class dummy_rrc_pdu_notifier : public rrc_pdu_notifier
{
public:
  dummy_rrc_pdu_notifier() = default;

  void on_new_pdu(const rrc_pdu_message& msg, ue_index_t old_ue_index) override
  {
    last_pdu      = byte_buffer_slice{msg.pdu};
    last_ue_index = old_ue_index;
  }

  byte_buffer_slice last_pdu;
  ue_index_t        last_ue_index;
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

  void on_ue_context_release_command(const rrc_ue_context_release_command& msg) override
  {
    logger.info("Received UE Context Release Command");
    last_rrc_ue_context_release_command.ue_index        = msg.ue_index;
    last_rrc_ue_context_release_command.cause           = msg.cause;
    last_rrc_ue_context_release_command.rrc_release_pdu = msg.rrc_release_pdu.copy();
  }

  async_task<bool> on_rrc_reestablishment_context_modification_required(ue_index_t ue_index) override
  {
    logger.info("Received Reestablishment Context Modification Required for ue={}", ue_index);

    return launch_async([](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  srb_creation_message           last_srb_creation_message;
  bool                           srb1_created = false;
  bool                           srb2_created = false;
  rrc_ue_context_release_command last_rrc_ue_context_release_command;

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

  void on_ue_context_release_request(const cu_cp_ue_context_release_request& msg) override
  {
    logger.info("Received UE Context Release Request");
  }

  bool initial_ue_msg_received = false;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

class dummy_rrc_ue_cu_cp_adapter : public rrc_ue_reestablishment_notifier
{
public:
  void add_ue_context(rrc_reestablishment_ue_context_t context) { reest_context = context; }

  rrc_reestablishment_ue_context_t
  on_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) override
  {
    logger.info("Received RRC Reestablishment Request from ue={} with old_pci={} and old_c_rnti={}",
                ue_index,
                old_pci,
                old_c_rnti);

    return reest_context;
  }

  void on_ue_transfer_required(ue_index_t ue_index, ue_index_t old_ue_index) override
  {
    logger.info("Requested a UE context transfer from ue={} with old_ue={}.", ue_index, old_ue_index);
  }

private:
  rrc_reestablishment_ue_context_t reest_context = {};
  srslog::basic_logger&            logger        = srslog::fetch_basic_logger("TEST");
};

class dummy_cell_meas_manager : public cell_meas_manager
{
public:
  optional<rrc_meas_cfg> get_measurement_config(nr_cell_id_t nci) override
  {
    optional<rrc_meas_cfg> meas_cfg;
    return meas_cfg;
  }
  optional<cell_meas_config> get_cell_config(nr_cell_id_t nci) override
  {
    optional<cell_meas_config> meas_cfg;
    return meas_cfg;
  }
  void update_cell_config(nr_cell_id_t                           nci,
                          const serving_cell_meas_config&        serv_cell_cfg_,
                          std::vector<neighbor_cell_meas_config> ncells_ = {}) override
  {
  }
  void report_measurement(const ue_index_t ue_index, const rrc_meas_results& meas_results) override {}
};

struct dummy_ue_task_scheduler : public rrc_ue_task_scheduler {
public:
  dummy_ue_task_scheduler(timer_manager& timers_, task_executor& exec_) : timer_db(timers_), exec(exec_) {}
  void          schedule_async_task(async_task<void>&& task) override { ctrl_loop.schedule(std::move(task)); }
  unique_timer  make_unique_timer() override { return timer_db.create_unique_timer(exec); }
  timer_factory get_timer_factory() override { return timer_factory{timer_db, exec}; }

  void tick_timer() { timer_db.tick(); }

private:
  async_task_sequencer ctrl_loop{16};
  timer_manager&       timer_db;
  task_executor&       exec;
};

} // namespace srs_cu_cp
} // namespace srsran
