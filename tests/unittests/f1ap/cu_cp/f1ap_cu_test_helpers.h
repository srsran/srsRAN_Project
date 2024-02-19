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
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_factory.h"
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

  void request_new_du_connection()
  {
    class sink_f1ap_message_notifier : public f1ap_message_notifier
    {
    public:
      void on_new_message(const f1ap_message& msg) override {}
    };

    auto notifier = local_f1c_gw.handle_du_connection_request(std::make_unique<sink_f1ap_message_notifier>());
    if (notifier != nullptr) {
      du_tx_notifiers.push_back(std::move(notifier));
    }
  }

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

/// Adapter between F1AP and CU-CP
class dummy_f1ap_ue_removal_notifier : public f1ap_ue_removal_notifier
{
public:
  void on_ue_removal_required(ue_index_t ue_index) override
  {
    logger.info("ue={}: Requested UE removal", ue_index);
    last_removed_ue = ue_index;
  }

private:
  ue_index_t            last_removed_ue = ue_index_t::invalid;
  srslog::basic_logger& logger          = srslog::fetch_basic_logger("TEST");
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
  dummy_f1ap_du_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  srs_cu_cp::du_index_t get_du_index() override { return srs_cu_cp::du_index_t::min; }

  du_setup_result on_new_du_setup_request(const du_setup_request& msg) override
  {
    logger.info("Received F1SetupRequest");
    last_f1_setup_request_msg = msg;
    return next_du_setup_resp;
  }

  srs_cu_cp::ue_rrc_context_creation_response
  on_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& msg) override
  {
    logger.info("Received {}", __FUNCTION__);
    last_ue_creation_msg.ue_index               = msg.ue_index;
    last_ue_creation_msg.cgi                    = msg.cgi;
    last_ue_creation_msg.du_to_cu_rrc_container = msg.du_to_cu_rrc_container.copy();
    last_ue_creation_msg.c_rnti                 = msg.c_rnti;

    srs_cu_cp::ue_rrc_context_creation_response ret = {};
    ret.f1ap_rrc_notifier                           = f1ap_rrc_notifier.get();

    return ret;
  }

  ue_index_t on_new_cu_cp_ue_required() override
  {
    ue_index_t ue_index = srs_cu_cp::ue_index_t::invalid;
    if (ue_id < srs_cu_cp::MAX_NOF_UES_PER_DU) {
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

  void set_ue_id(uint16_t ue_id_) { ue_id = ue_id_; }

  srs_cu_cp::du_setup_request last_f1_setup_request_msg;
  srs_cu_cp::du_setup_result  next_du_setup_resp;

  srs_cu_cp::ue_rrc_context_creation_request       last_ue_creation_msg;
  optional<srs_cu_cp::ue_index_t>                  last_created_ue_index;
  std::unique_ptr<dummy_f1ap_rrc_message_notifier> f1ap_rrc_notifier =
      std::make_unique<dummy_f1ap_rrc_message_notifier>();

private:
  srslog::basic_logger& logger;
  uint16_t              ue_id = ue_index_to_uint(srs_cu_cp::ue_index_t::min);
};

/// Reusable notifier class that a) stores the received du_index for test inspection and b)
/// calls the registered DU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1ap_du_management_notifier : public f1ap_du_management_notifier
{
public:
  void attach_handler(cu_cp_f1c_handler* handler_) { handler = handler_; };

  void on_du_remove_request_received(du_index_t idx) override
  {
    logger.info("Received a DU remove request for du={}", idx);
    last_du_idx = idx; // store idx

    if (handler != nullptr) {
      logger.info("Forwarding remove request");
      handler->handle_du_remove_request(idx);
    }
  }

  du_index_t last_du_idx;

private:
  srslog::basic_logger& logger  = srslog::fetch_basic_logger("TEST");
  cu_cp_f1c_handler*    handler = nullptr;
};

/// \brief Creates a dummy UE CONTEXT SETUP REQUEST.
f1ap_ue_context_setup_request create_ue_context_setup_request(const std::initializer_list<drb_id_t>& drbs_to_add);

/// Fixture class for F1AP
class f1ap_cu_test : public ::testing::Test
{
protected:
  struct test_ue {
    ue_index_t                    ue_index;
    optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
    optional<gnb_du_ue_f1ap_id_t> du_ue_id;
  };

  f1ap_cu_test();
  ~f1ap_cu_test() override;

  /// \brief Helper method to successfully create UE instance in F1AP.
  test_ue& create_ue(gnb_du_ue_f1ap_id_t du_ue_id);

  /// \brief Helper method to run F1AP CU UE Context Setup procedure to completion for a given UE.
  test_ue& run_ue_context_setup();

  srslog::basic_logger& f1ap_logger = srslog::fetch_basic_logger("CU-CP-F1");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  std::unordered_map<ue_index_t, test_ue> test_ues;

  dummy_f1ap_pdu_notifier           f1ap_pdu_notifier;
  dummy_f1ap_du_processor_notifier  du_processor_notifier;
  dummy_f1ap_du_management_notifier f1ap_du_mgmt_notifier;
  dummy_f1ap_ue_removal_notifier    f1ap_cu_cp_notifier;
  timer_manager                     timers;
  manual_task_worker                ctrl_worker{128};
  std::unique_ptr<f1ap_cu>          f1ap;
};

} // namespace srs_cu_cp
} // namespace srsran
