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

#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/f1ap/du/f1c_connection_client.h"

namespace srsran {
namespace srs_du {

class f1ap_du_connection_handler : public f1ap_message_notifier
{
public:
  f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_, f1ap_message_handler& f1ap_pdu_handler_);

  SRSRAN_NODISCARD bool connect_to_cu_cp();
  SRSRAN_NODISCARD bool is_connected() const { return f1ap_notifier != nullptr; }

  void on_new_message(const srsran::f1ap_message& msg) override;

private:
  f1c_connection_client& f1c_client_handler;
  f1ap_message_handler&  f1ap_pdu_handler;
  srslog::basic_logger&  logger;

  std::unique_ptr<f1ap_message_notifier> f1ap_notifier;
};

} // namespace srs_du
} // namespace srsran