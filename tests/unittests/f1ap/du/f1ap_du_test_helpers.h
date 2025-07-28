/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/du/du_high/du_manager/converters/f1ap_configuration_helpers.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "srsran/f1ap/du/f1ap_du_positioning_handler.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsran::srs_du {

/// \brief Generate a random gnb_du_ue_f1ap_id
gnb_du_ue_f1ap_id_t generate_random_gnb_du_ue_f1ap_id();

class dummy_f1ap_du_configurator : public f1ap_du_configurator, public f1ap_du_positioning_handler
{
public:
  struct dummy_ue_task_sched : public f1ap_ue_task_scheduler {
    dummy_f1ap_du_configurator* parent;

    explicit dummy_ue_task_sched(dummy_f1ap_du_configurator* parent_) : parent(parent_) {}

    unique_timer create_timer() override { return parent->timers.create_timer(); }

    /// \brief Schedule Async Task respective to a given UE.
    void schedule_async_task(async_task<void>&& task) override { parent->task_loop.schedule(std::move(task)); }
  };

  timer_factory             timers;
  fifo_async_task_scheduler task_loop;
  dummy_ue_task_sched       ue_sched;
  f1ap_du*                  f1ap;

  // DU manager -> F1AP.
  f1ap_ue_creation_request                               next_ue_creation_req;
  std::optional<f1ap_ue_creation_response>               last_ue_creation_response;
  f1ap_ue_configuration_request                          next_ue_cfg_req;
  std::optional<f1ap_ue_configuration_response>          last_ue_cfg_response;
  std::optional<std::pair<du_ue_index_t, du_ue_index_t>> last_reestablishment_ue_indexes;
  du_positioning_info_response                           next_positioning_info_response;

  // F1AP procedures.
  std::optional<gnbcu_config_update_request>      last_cu_upd_req;
  gnbcu_config_update_response                    next_cu_upd_resp;
  std::optional<f1ap_ue_context_creation_request> last_ue_context_creation_req;
  f1ap_ue_context_creation_response               next_ue_context_creation_response;
  std::optional<f1ap_ue_context_update_request>   last_ue_context_update_req;
  f1ap_ue_context_update_response                 next_ue_context_update_response;
  std::optional<f1ap_ue_delete_request>           last_ue_delete_req;
  std::optional<du_ue_index_t>                    last_ue_cfg_applied;
  std::optional<std::vector<du_ue_index_t>>       last_ues_to_reset;
  std::optional<du_positioning_info_request>      last_positioning_info_request;
  std::optional<du_positioning_meas_request>      last_positioning_meas_request;

  explicit dummy_f1ap_du_configurator(timer_factory& timers_) : timers(timers_), task_loop(128), ue_sched(this) {}

  void connect(f1ap_du& f1ap_) { f1ap = &f1ap_; }

  timer_factory& get_timer_factory() override { return timers; }

  void schedule_async_task(async_task<void>&& task) override { task_loop.schedule(std::move(task)); }

  void on_f1c_disconnection() override {}

  async_task<void> request_reset(const std::vector<du_ue_index_t>& ues_to_reset) override
  {
    last_ues_to_reset = ues_to_reset;
    return launch_no_op_task();
  }

  async_task<gnbcu_config_update_response>
  request_cu_context_update(const gnbcu_config_update_request& request) override
  {
    last_cu_upd_req = request;
    return launch_no_op_task(next_cu_upd_resp);
  }

  du_ue_index_t find_free_ue_index() override { return next_ue_creation_req.ue_index; }

  async_task<f1ap_ue_context_creation_response>
  request_ue_creation(const f1ap_ue_context_creation_request& request) override
  {
    last_ue_context_creation_req = request;
    return launch_async([this](coro_context<async_task<f1ap_ue_context_creation_response>>& ctx) {
      CORO_BEGIN(ctx);
      last_ue_creation_response = f1ap->handle_ue_creation_request(next_ue_creation_req);
      CORO_RETURN(next_ue_context_creation_response);
    });
  }

  async_task<f1ap_ue_context_update_response>
  request_ue_context_update(const f1ap_ue_context_update_request& request) override
  {
    last_ue_context_update_req = request;
    return launch_async([this](coro_context<async_task<f1ap_ue_context_update_response>>& ctx) {
      CORO_BEGIN(ctx);
      last_ue_cfg_response = f1ap->handle_ue_configuration_request(next_ue_cfg_req);
      CORO_RETURN(next_ue_context_update_response);
    });
  }

  async_task<void> request_ue_removal(const f1ap_ue_delete_request& request) override
  {
    last_ue_delete_req = request;
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  void on_ue_config_applied(du_ue_index_t ue_index) override { last_ue_cfg_applied = ue_index; }

  async_task<void> request_ue_drb_deactivation(du_ue_index_t ue_index) override { return launch_no_op_task(); }

  void notify_reestablishment_of_old_ue(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) override
  {
    last_reestablishment_ue_indexes = std::make_pair(new_ue_index, old_ue_index);
  }

  f1ap_du_positioning_handler& get_positioning_handler() override { return *this; }

  du_trp_info_response request_trp_info() override
  {
    return du_trp_info_response{
        {{.trp_id = trp_id_t{1}, .pci = MIN_PCI},
         {.trp_id = trp_id_t{1}, .cgi = nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::min()}},
         {.trp_id = trp_id_t{1}, .arfcn = 368500}}};
  }

  async_task<du_positioning_info_response> request_positioning_info(const du_positioning_info_request& req) override
  {
    last_positioning_info_request = req;
    return launch_no_op_task(du_positioning_info_response{next_positioning_info_response});
  }

  async_task<du_positioning_meas_response>
  request_positioning_measurement(const du_positioning_meas_request& req) override
  {
    last_positioning_meas_request = req;
    return launch_no_op_task(du_positioning_meas_response{});
  }

  /// \brief Retrieve task scheduler specific to a given UE.
  f1ap_ue_task_scheduler& get_ue_handler(du_ue_index_t ue_index) override { return ue_sched; }
};

class dummy_ue_executor_mapper : public du_high_ue_executor_mapper
{
public:
  dummy_ue_executor_mapper(task_executor& exec_) : exec(exec_) {}

  void           rebind_executors(du_ue_index_t ue_index, du_cell_index_t pcell_index) override {}
  task_executor& ctrl_executor(du_ue_index_t ue_index) override { return exec; }
  task_executor& f1u_dl_pdu_executor(du_ue_index_t ue_index) override { return exec; }
  task_executor& mac_ul_pdu_executor(du_ue_index_t ue_index) override { return exec; }

  task_executor& exec;
};

f1_setup_request_message generate_f1_setup_request_message();

/// \brief Generate F1AP ASN.1 DRB AM Setup configuration.
asn1::f1ap::drbs_to_be_setup_item_s generate_drb_am_setup_item(drb_id_t drbid);

/// \brief Generate F1AP ASN.1 DRB AM Setup configuration.
asn1::f1ap::drbs_to_be_setup_mod_item_s generate_drb_am_mod_item(drb_id_t drbid);

/// \brief Generate an F1AP UE Context Release Command message.
f1ap_message generate_ue_context_release_command();

/// \brief Generate an F1AP DL RRC Message Transfer message.
f1ap_message generate_dl_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                              gnb_cu_ue_f1ap_id_t cu_ue_id,
                                              srb_id_t            srb_id,
                                              byte_buffer         rrc_container);

class dummy_f1c_connection_client : public srs_du::f1c_connection_client
{
public:
  bool                        tx_pdus_sent() const { return not tx_f1ap_pdus.empty(); }
  const f1ap_message&         last_tx_pdu() const { return tx_f1ap_pdus.back(); }
  std::optional<f1ap_message> pop_tx_pdu()
  {
    std::optional<f1ap_message> ret;
    if (tx_f1ap_pdus.empty()) {
      return ret;
    }
    ret = tx_f1ap_pdus.front();
    tx_f1ap_pdus.pop_front();
    return ret;
  }
  void clear_tx_pdus() { tx_f1ap_pdus.clear(); }

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

private:
  std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier;
  std::deque<f1ap_message>               tx_f1ap_pdus;
};

class dummy_f1c_rx_sdu_notifier : public f1c_rx_sdu_notifier
{
public:
  byte_buffer last_pdu;

  void on_new_sdu(byte_buffer pdu) override { last_pdu = std::move(pdu); }
};

class dummy_f1u_rx_sdu_notifier : public f1u_rx_sdu_notifier
{
public:
  byte_buffer             last_pdu;
  bool                    last_pdu_is_retx;
  std::optional<uint32_t> last_discard_sn;

  void on_new_sdu(byte_buffer sdu, bool is_retx) override
  {
    last_pdu         = std::move(sdu);
    last_pdu_is_retx = is_retx;
  }

  void on_discard_sdu(uint32_t pdcp_sn) override { last_discard_sn = pdcp_sn; }
};

class dummy_mac_f1ap_paging_handler : public f1ap_du_paging_notifier
{
public:
  void connect(mac_paging_information_handler& mac_) { mac = &mac_; }

  void on_paging_received(const paging_information& msg) override { mac->handle_paging_information(msg); }

private:
  mac_paging_information_handler* mac;
};

/// Fixture class for F1AP
class f1ap_du_test : public ::testing::Test
{
public:
  ~f1ap_du_test();

protected:
  struct f1c_test_bearer {
    // This user provided constructor is added here to fix a Clang compilation error related to the use of nested
    // types with std::optional.
    f1c_test_bearer() {}

    srb_id_t                  srb_id = srb_id_t::nulltype;
    dummy_f1c_rx_sdu_notifier rx_sdu_notifier;
    f1c_bearer*               bearer = nullptr;
  };
  struct f1u_test_bearer {
    // This user provided constructor is added here to fix a Clang compilation error related to the use of nested
    // types with std::optional.
    f1u_test_bearer() {}

    drb_id_t                  drb_id = drb_id_t::invalid;
    dummy_f1u_rx_sdu_notifier rx_sdu_notifier;
    f1u_bearer*               bearer = nullptr;
  };
  struct ue_test_context {
    du_ue_index_t                                ue_index;
    rnti_t                                       crnti;
    std::optional<gnb_du_ue_f1ap_id_t>           gnb_du_ue_f1ap_id;
    std::optional<gnb_cu_ue_f1ap_id_t>           gnb_cu_ue_f1ap_id;
    slotted_array<f1c_test_bearer, MAX_NOF_SRBS> f1c_bearers;
    slotted_array<f1u_test_bearer, MAX_NOF_DRBS> f1u_bearers;
  };

  f1ap_du_test();

  /// \brief Run F1 Setup Procedure to completion.
  void run_f1_setup_procedure();

  void run_f1_removal_procedure();

  /// \brief Create new UE in F1AP.
  ue_test_context* run_f1ap_ue_create(du_ue_index_t ue_index);

  void run_ue_context_setup_procedure(du_ue_index_t ue_index, const f1ap_message& msg);

  /// \brief Update UE config in F1AP.
  f1ap_ue_configuration_response update_f1ap_ue_config(du_ue_index_t                   ue_index,
                                                       std::initializer_list<srb_id_t> srbs,
                                                       std::initializer_list<drb_id_t> drbs);

  void tick();

  /// Dummy F1-C gateway to connect to CU-CP and send F1AP PDUs.
  dummy_f1c_connection_client f1c_gw;

  timer_manager                 timer_service;
  timer_factory                 f1ap_timers{timer_service, ctrl_worker};
  dummy_f1ap_du_configurator    f1ap_du_cfg_handler{f1ap_timers};
  manual_task_worker            ctrl_worker{128};
  dummy_ue_executor_mapper      ue_exec_mapper{ctrl_worker};
  dummy_mac_f1ap_paging_handler paging_handler;
  std::unique_ptr<f1ap_du>      f1ap;

  /// Storage of UE context related to the current unit test.
  slotted_array<ue_test_context, MAX_NOF_DU_UES> test_ues;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

} // namespace srsran::srs_du
