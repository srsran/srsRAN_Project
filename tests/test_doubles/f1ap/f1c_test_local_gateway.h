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

#include "srsran/cu_cp/du_repository.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/du/f1c_connection_client.h"

namespace srsran {

/// \brief F1AP message notifier that stores the last received PDU.
class test_f1ap_message_notifier : public f1ap_message_notifier
{
public:
  test_f1ap_message_notifier(bool                                   cu_to_du_dir_,
                             std::vector<f1ap_message>*             last_pdus_,
                             std::unique_ptr<f1ap_message_notifier> decorated = nullptr) :
    cu_to_du_dir(cu_to_du_dir_), last_pdus(last_pdus_), notifier(std::move(decorated))
  {
  }

  void on_new_message(const f1ap_message& msg) override
  {
    if (cu_to_du_dir) {
      logger.info("F1-C Forwarding CU-CP > DU: {}", msg.pdu.type().to_string());
    } else {
      logger.info("F1-C Forwarding DU > CU-CP: {}", msg.pdu.type().to_string());
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
  bool                                   cu_to_du_dir;
  std::vector<f1ap_message>*             last_pdus;
  std::unique_ptr<f1ap_message_notifier> notifier;
};

/// \brief Test helper class that creates an F1-C gateway for co-located setups (CU-CP and DU in the same process), and
/// stores the messages received by the CU-CP and DU for testing purposes.
class f1c_test_local_gateway : public srs_du::f1c_connection_client
{
public:
  f1c_test_local_gateway() = default;
  explicit f1c_test_local_gateway(srs_cu_cp::du_repository& cu_cp_du_mng_) : cu_cp_du_mng(&cu_cp_du_mng_) {}

  void attach_cu_cp_du_repo(srs_cu_cp::du_repository& cu_cp_du_mng_) { cu_cp_du_mng = &cu_cp_du_mng_; }

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override
  {
    connections.emplace_back(std::make_unique<du_connection_test_context>());
    auto& conn = connections.back();

    auto decorated_du_rx_pdu_notifier =
        std::make_unique<test_f1ap_message_notifier>(true, &conn->last_du_rx_pdus, std::move(du_rx_pdu_notifier));

    auto cu_rx_pdu_notifier = cu_cp_du_mng->handle_new_du_connection(std::move(decorated_du_rx_pdu_notifier));

    return std::make_unique<test_f1ap_message_notifier>(
        false, &conn->last_cu_cp_rx_pdus, std::move(cu_rx_pdu_notifier));
  }

  span<const f1ap_message> get_last_cu_cp_rx_pdus(std::size_t connection_index) const
  {
    return connections.at(connection_index)->last_cu_cp_rx_pdus;
  }
  span<const f1ap_message> get_last_cu_cp_tx_pdus(std::size_t connection_index) const
  {
    return connections.at(connection_index)->last_du_rx_pdus;
  }

  void clear_messages()
  {
    for (unsigned i = 0; i != connections.size(); ++i) {
      connections.at(i)->last_cu_cp_rx_pdus.clear();
      connections.at(i)->last_du_rx_pdus.clear();
    }
  }

private:
  struct du_connection_test_context {
    /// Last messages sent by the DU and received by the CU-CP.
    std::vector<f1ap_message> last_cu_cp_rx_pdus;

    /// Last messages sent by the CU-CP and received by the DU.
    std::vector<f1ap_message> last_du_rx_pdus;
  };

  srs_cu_cp::du_repository* cu_cp_du_mng = nullptr;

  std::vector<std::unique_ptr<du_connection_test_context>> connections;
};

} // namespace srsran
