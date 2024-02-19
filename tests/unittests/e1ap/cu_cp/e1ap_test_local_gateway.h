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

#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_up/e1ap_connection_client.h"

namespace srsran {

/// \brief E1AP message notifier that stores the last received PDU.
class test_e1ap_message_notifier : public e1ap_message_notifier
{
public:
  test_e1ap_message_notifier(bool                                   cu_cp_to_cu_up_dir_,
                             std::vector<e1ap_message>*             last_pdus_,
                             std::unique_ptr<e1ap_message_notifier> decorated = nullptr) :
    cu_cp_to_cu_up_dir(cu_cp_to_cu_up_dir_), last_pdus(last_pdus_), notifier(std::move(decorated))
  {
  }

  void on_new_message(const e1ap_message& msg) override
  {
    if (cu_cp_to_cu_up_dir) {
      logger.info("E1AP Forwarding CU-CP > CU-UP: {}", msg.pdu.type().to_string());
    } else {
      logger.info("E1AP Forwarding CU-UP > CU-CP: {}", msg.pdu.type().to_string());
    }
    if (last_pdus != nullptr) {
      last_pdus->push_back(msg);
    }
    if (notifier != nullptr) {
      notifier->on_new_message(msg);
    }
  }

private:
  srslog::basic_logger&                  logger = srslog::fetch_basic_logger("TEST");
  bool                                   cu_cp_to_cu_up_dir;
  std::vector<e1ap_message>*             last_pdus;
  std::unique_ptr<e1ap_message_notifier> notifier;
};

/// \brief Test helper class that creates an E1AP gateway for co-located setups (CU-CP and CU-UP in the same process),
/// and stores the messages received by the CU-CP and CU-UP for testing purposes.
class e1ap_test_local_gateway : public srs_cu_up::e1ap_connection_client
{
public:
  e1ap_test_local_gateway() = default;
  explicit e1ap_test_local_gateway(srs_cu_cp::cu_cp_e1_handler& cu_cp_cu_up_mng_) : cu_cp_cu_up_mng(&cu_cp_cu_up_mng_)
  {
  }

  void attach_cu_cp_cu_up_repo(srs_cu_cp::cu_cp_e1_handler& cu_cp_cu_up_mng_) { cu_cp_cu_up_mng = &cu_cp_cu_up_mng_; }

  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier) override
  {
    connections.emplace_back(std::make_unique<cu_up_connection_test_context>());
    auto& conn = connections.back();

    auto decorated_cu_up_rx_pdu_notifier =
        std::make_unique<test_e1ap_message_notifier>(true, &conn->last_cu_up_rx_pdus, std::move(cu_up_rx_pdu_notifier));

    auto cu_rx_pdu_notifier = cu_cp_cu_up_mng->handle_new_cu_up_connection(std::move(decorated_cu_up_rx_pdu_notifier));

    return std::make_unique<test_e1ap_message_notifier>(
        false, &conn->last_cu_cp_rx_pdus, std::move(cu_rx_pdu_notifier));
  }

  span<const e1ap_message> get_last_cu_cp_rx_pdus(std::size_t connection_index) const
  {
    return connections.at(connection_index)->last_cu_cp_rx_pdus;
  }
  span<const e1ap_message> get_last_cu_cp_tx_pdus(std::size_t connection_index) const
  {
    return connections.at(connection_index)->last_cu_up_rx_pdus;
  }

  void clear_messages()
  {
    for (unsigned i = 0; i != connections.size(); ++i) {
      connections.at(i)->last_cu_cp_rx_pdus.clear();
      connections.at(i)->last_cu_up_rx_pdus.clear();
    }
  }

private:
  struct cu_up_connection_test_context {
    /// Last messages sent by the CU-UP and received by the CU-CP.
    std::vector<e1ap_message> last_cu_cp_rx_pdus;

    /// Last messages sent by the CU-CP and received by the CU-UP.
    std::vector<e1ap_message> last_cu_up_rx_pdus;
  };

  srs_cu_cp::cu_cp_e1_handler* cu_cp_cu_up_mng = nullptr;

  std::vector<std::unique_ptr<cu_up_connection_test_context>> connections;
};

} // namespace srsran
