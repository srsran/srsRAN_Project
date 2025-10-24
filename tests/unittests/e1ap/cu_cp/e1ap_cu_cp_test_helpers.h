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

#include "../common/e1ap_cu_cp_test_messages.h"
#include "../common/test_helpers.h"
#include "e1_test_local_gateway.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// Dummy notifier just printing the received msg.
class dummy_e1ap_cu_cp_notifier : public srs_cu_cp::e1ap_cu_cp_notifier
{
public:
  dummy_e1ap_cu_cp_notifier(srs_cu_cp::ue_manager& ue_mng_) :
    ue_mng(ue_mng_), logger(srslog::fetch_basic_logger("TEST"))
  {
  }

  void on_bearer_context_release_request_received(const srs_cu_cp::cu_cp_bearer_context_release_request& msg) override
  {
    last_release_request = msg;
    logger.info("Received a bearer context release request");
  }

  void on_bearer_context_inactivity_notification_received(const srs_cu_cp::cu_cp_inactivity_notification& msg) override
  {
    last_msg = msg;
    logger.info("Received an inactivity notification");
  }

  async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) override
  {
    logger.info("ue={}: Requested a UE release", request.ue_index);
    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  void on_e1_release_request_received(cu_up_index_t cu_up_index) override
  {
    logger.info("Received E1 Release Request for CU-UP {}", cu_up_index);
  }

  bool schedule_async_task(ue_index_t ue_index, async_task<void> task) override
  {
    srsran_assert(ue_mng.find_ue_task_scheduler(ue_index) != nullptr, "UE task scheduler must be present");
    return ue_mng.find_ue_task_scheduler(ue_index)->schedule_async_task(std::move(task));
  }

  async_task<void> on_transaction_info_loss(const ue_transaction_info_loss_event& ev) override
  {
    logger.info("Received transaction info loss for {} UEs", ev.ues_lost.size());
    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  srs_cu_cp::cu_cp_bearer_context_release_request last_release_request;
  srs_cu_cp::cu_cp_inactivity_notification        last_msg;

private:
  ue_manager&           ue_mng;
  srslog::basic_logger& logger;
};

/// \brief Reusable E1AP gateway test class for CU-CP unit tests. This class includes:
/// a) Requests a new CU-UP connection to the CU-CP.
/// b) Logs and stores the last transmitted/received PDU by/from the CU-CP.
class dummy_cu_cp_e1ap_gateway
{
public:
  dummy_cu_cp_e1ap_gateway() : logger(srslog::fetch_basic_logger("TEST")) {}

  void attach_cu_cp_cu_up_repo(srs_cu_cp::cu_cp_e1_handler& cu_cp_cu_up_mng_)
  {
    local_e1ap_gw.attach_cu_cp_cu_up_repo(cu_cp_cu_up_mng_);
  }

  void request_new_cu_up_connection()
  {
    class sink_e1ap_message_notifier : public e1ap_message_notifier
    {
    public:
      void on_new_message(const e1ap_message& msg) override {}
    };

    auto notifier = local_e1ap_gw.handle_cu_up_connection_request(std::make_unique<sink_e1ap_message_notifier>());
    if (notifier != nullptr) {
      cu_up_tx_notifiers.push_back(std::move(notifier));
    }
  }

  void remove_cu_up_connection(size_t connection_idx)
  {
    cu_up_tx_notifiers.erase(cu_up_tx_notifiers.begin() + connection_idx);
  }

  e1ap_message_notifier& get_cu_up(size_t connection_idx) { return *cu_up_tx_notifiers.at(connection_idx); }

  span<const e1ap_message> last_rx_pdus(size_t connection_idx) const
  {
    return local_e1ap_gw.get_last_cu_cp_rx_pdus(connection_idx);
  }
  span<const e1ap_message> last_tx_pdus(size_t connection_idx) const
  {
    return local_e1ap_gw.get_last_cu_cp_tx_pdus(connection_idx);
  }

  void push_cu_cp_rx_pdu(size_t cu_up_connectin_idx, const e1ap_message& msg)
  {
    cu_up_tx_notifiers[cu_up_connectin_idx]->on_new_message(msg);
  }

  size_t nof_connections() const { return cu_up_tx_notifiers.size(); }

private:
  srslog::basic_logger& logger;
  e1_test_local_gateway local_e1ap_gw;

  std::vector<std::unique_ptr<e1ap_message_notifier>> cu_up_tx_notifiers;
};

/// Fixture class for E1AP
class e1ap_cu_cp_test : public ::testing::Test
{
protected:
  struct test_ue {
    ue_index_t                            ue_index;
    std::optional<gnb_cu_cp_ue_e1ap_id_t> cu_cp_ue_e1ap_id;
    std::optional<gnb_cu_up_ue_e1ap_id_t> cu_up_ue_e1ap_id;
  };

  e1ap_cu_cp_test();
  ~e1ap_cu_cp_test() override;

  /// \brief Helper method to run E1AP CU-CP Bearer Context Setup procedure.
  void run_bearer_context_setup(ue_index_t ue_index, gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

  /// \brief Helper method to create a E1AP UE
  test_ue& create_ue();

  void tick();

  srslog::basic_logger& e1ap_logger = srslog::fetch_basic_logger("E1AP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  std::unordered_map<ue_index_t, test_ue> test_ues;

  timer_manager                       timers;
  dummy_e1ap_pdu_notifier             e1ap_pdu_notifier;
  dummy_e1ap_cu_up_processor_notifier cu_up_processor_notifier;
  manual_task_worker                  ctrl_worker{128};
  cu_cp_configuration                 cu_cp_cfg;

  ue_manager                  ue_mng{cu_cp_cfg};
  dummy_e1ap_cu_cp_notifier   cu_cp_notifier{ue_mng};
  std::unique_ptr<e1ap_cu_cp> e1ap;
};

} // namespace srs_cu_cp
} // namespace srsran
