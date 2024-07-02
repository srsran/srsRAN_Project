/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../common/f1ap_cu_test_messages.h"
#include "../common/test_helpers.h"
#include "tests/test_doubles/f1ap/f1c_test_local_gateway.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate a random gnb_cu_ue_f1ap_id
gnb_cu_ue_f1ap_id_t generate_random_gnb_cu_ue_f1ap_id();

/// \brief Reusable F1-C gateway test class for CU-CP unit tests. This class includes:
/// a) Requests a new DU connection to the CU-CP.
/// b) Logs and stores the last transmitted/received PDU by/from the CU-CP.
class dummy_cu_cp_f1c_gateway
{
public:
  dummy_cu_cp_f1c_gateway() : logger(srslog::fetch_basic_logger("TEST")) {}

  void attach_cu_cp_du_repo(srs_cu_cp::cu_cp_f1c_handler& cu_cp_du_mng_)
  {
    local_f1c_gw.attach_cu_cp_du_repo(cu_cp_du_mng_);
  }

  f1ap_message_notifier* request_new_du_connection()
  {
    class sink_f1ap_message_notifier : public f1ap_message_notifier
    {
    public:
      void on_new_message(const f1ap_message& msg) override {}
    };

    auto notifier = local_f1c_gw.handle_du_connection_request(std::make_unique<sink_f1ap_message_notifier>());
    if (notifier != nullptr) {
      du_tx_notifiers.push_back(std::move(notifier));
      return du_tx_notifiers.back().get();
    }
    return nullptr;
  }

  f1ap_message_notifier& get_du(du_index_t du_idx) { return *du_tx_notifiers.at((unsigned)du_idx); }

  void remove_du_connection(size_t connection_idx) { du_tx_notifiers.erase(du_tx_notifiers.begin() + connection_idx); }

  span<const f1ap_message> last_rx_pdus(size_t connection_idx) const
  {
    return local_f1c_gw.get_last_cu_cp_rx_pdus(connection_idx);
  }
  span<const f1ap_message> last_tx_pdus(size_t connection_idx) const
  {
    return local_f1c_gw.get_last_cu_cp_tx_pdus(connection_idx);
  }

  void push_cu_cp_rx_pdu(size_t du_connectin_idx, const f1ap_message& msg)
  {
    du_tx_notifiers[du_connectin_idx]->on_new_message(msg);
  }

  size_t nof_connections() const { return du_tx_notifiers.size(); }

private:
  srslog::basic_logger&  logger;
  f1c_test_local_gateway local_f1c_gw;

  std::vector<std::unique_ptr<f1ap_message_notifier>> du_tx_notifiers;
};

class dummy_f1ap_rrc_message_notifier : public srs_cu_cp::f1ap_rrc_message_notifier
{
public:
  dummy_f1ap_rrc_message_notifier() = default;
  void on_ul_ccch_pdu(byte_buffer pdu) override
  {
    logger.info("Received UL CCCH RRC message");
    last_ul_ccch_pdu = std::move(pdu);
  };

  void on_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) override
  {
    logger.info("Received UL DCCH RRC {} message.", srb_id);
    last_srb_id      = srb_id;
    last_ul_dcch_pdu = std::move(pdu);
  };

  byte_buffer last_ul_ccch_pdu;
  byte_buffer last_ul_dcch_pdu;
  srb_id_t    last_srb_id;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

class dummy_f1ap_du_processor_notifier : public srs_cu_cp::f1ap_du_processor_notifier
{
public:
  dummy_f1ap_du_processor_notifier(const unsigned max_nof_supported_ues_) :
    max_nof_supported_ues(max_nof_supported_ues_), logger(srslog::fetch_basic_logger("TEST"))
  {
  }

  du_setup_result on_new_du_setup_request(const du_setup_request& msg) override
  {
    logger.info("Received F1SetupRequest");
    last_f1_setup_request_msg = msg;
    return next_du_setup_resp;
  }

  srs_cu_cp::ue_rrc_context_creation_outcome
  on_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& msg) override
  {
    logger.info("Received {}", __FUNCTION__);
    last_ue_creation_msg.ue_index               = msg.ue_index;
    last_ue_creation_msg.cgi                    = msg.cgi;
    last_ue_creation_msg.du_to_cu_rrc_container = msg.du_to_cu_rrc_container.copy();
    last_ue_creation_msg.c_rnti                 = msg.c_rnti;

    srs_cu_cp::ue_rrc_context_creation_response response{msg.ue_index, f1ap_rrc_notifier.get()};
    if (msg.ue_index == ue_index_t::invalid) {
      response.ue_index = on_new_cu_cp_ue_required();
    }

    // Return failure if no UE index is available.
    if (response.ue_index == ue_index_t::invalid) {
      return make_unexpected(byte_buffer::create({0x0, 0x0}).value());
    }

    return response;
  }

  ue_index_t on_new_cu_cp_ue_required()
  {
    ue_index_t ue_index = srs_cu_cp::ue_index_t::invalid;
    if (ue_id < max_nof_supported_ues) {
      ue_index              = srs_cu_cp::uint_to_ue_index(ue_id);
      last_created_ue_index = ue_index;
      ue_id++;
    }

    return ue_index;
  }

  void on_du_initiated_ue_context_release_request(const srs_cu_cp::f1ap_ue_context_release_request& req) override
  {
    logger.info("Received UEContextReleaseRequest");
    // TODO
  }

  bool schedule_async_task(async_task<void> task) override { return task_sched.schedule(std::move(task)); }

  async_task<void> on_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev) override
  {
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  void set_ue_id(uint16_t ue_id_) { ue_id = ue_id_; }

  srs_cu_cp::du_setup_request last_f1_setup_request_msg;
  srs_cu_cp::du_setup_result  next_du_setup_resp;

  srs_cu_cp::ue_rrc_context_creation_request       last_ue_creation_msg;
  std::optional<srs_cu_cp::ue_index_t>             last_created_ue_index;
  std::unique_ptr<dummy_f1ap_rrc_message_notifier> f1ap_rrc_notifier =
      std::make_unique<dummy_f1ap_rrc_message_notifier>();

private:
  const unsigned            max_nof_supported_ues;
  srslog::basic_logger&     logger;
  uint16_t                  ue_id = ue_index_to_uint(srs_cu_cp::ue_index_t::min);
  fifo_async_task_scheduler task_sched{16};
};

/// \brief Creates a dummy UE CONTEXT SETUP REQUEST.
f1ap_ue_context_setup_request create_ue_context_setup_request(const std::initializer_list<drb_id_t>& drbs_to_add);

/// Fixture class for F1AP
class f1ap_cu_test : public ::testing::Test
{
protected:
  struct test_ue {
    ue_index_t                         ue_index;
    std::optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
    std::optional<gnb_du_ue_f1ap_id_t> du_ue_id;
  };

  f1ap_cu_test(const f1ap_configuration& f1ap_cfg = {});
  ~f1ap_cu_test() override;

  /// \brief Helper method to successfully create UE instance in F1AP.
  test_ue& create_ue(gnb_du_ue_f1ap_id_t du_ue_id);

  /// \brief Helper method to run F1AP CU UE Context Setup procedure to completion for a given UE.
  test_ue& run_ue_context_setup();

  bool was_rrc_reject_sent();

  void tick();

  srslog::basic_logger& f1ap_logger = srslog::fetch_basic_logger("CU-CP-F1");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  std::unordered_map<ue_index_t, test_ue> test_ues;

  const unsigned max_nof_ues = 8192;

  dummy_f1ap_pdu_notifier          f1ap_pdu_notifier;
  dummy_f1ap_du_processor_notifier du_processor_notifier{max_nof_ues};
  timer_manager                    timers;
  manual_task_worker               ctrl_worker{128};
  std::unique_ptr<f1ap_cu>         f1ap;
};

} // namespace srs_cu_cp
} // namespace srsran
