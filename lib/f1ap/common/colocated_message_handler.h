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

#include "srsran/f1ap/du/f1ap_du.h"

namespace srsran {

/// This implementation forwards F1AP messages through a function call to the peer F1 interface. It is useful for
/// co-located scenarios where both F1 interfaces reside in the same process.
class colocated_f1ap_pdu_handler : public f1ap_message_handler
{
  f1ap_message_handler& peer_handler;

public:
  explicit colocated_f1ap_pdu_handler(f1ap_message_handler& peer_handler) : peer_handler(peer_handler) {}

  void handle_message(const asn1::f1ap::f1ap_pdu_c& msg) override { peer_handler.handle_message(msg); }
};

} // namespace srsran
