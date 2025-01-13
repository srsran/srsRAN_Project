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

#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/rb_id.h"

namespace srsran {
namespace srs_cu_cp {

struct f1ap_dl_rrc_message {
  ue_index_t  ue_index = ue_index_t::invalid;
  srb_id_t    srb_id   = srb_id_t::nulltype;
  byte_buffer rrc_container;
};

/// Handles incoming DL RRC messages to be transmitted over the F1 interface.
class f1ap_rrc_message_handler
{
public:
  virtual ~f1ap_rrc_message_handler() = default;

  /// Handles the given DL RRC message.
  virtual void handle_dl_rrc_message_transfer(const f1ap_dl_rrc_message& msg) = 0;
};

/// Notifier interface used to notify outgoing UL CCCH PDUs that were received over the F1 interface.
class f1ap_ul_ccch_notifier
{
public:
  virtual ~f1ap_ul_ccch_notifier() = default;

  /// This callback is invoked on each outgoing UL CCCH PDU.
  virtual void on_ul_ccch_pdu(byte_buffer pdu) = 0;
};

/// Notifier interface used to notify outgoing UL DCCH PDUs that were received over the F1 interface.
class f1ap_ul_dcch_notifier
{
public:
  virtual ~f1ap_ul_dcch_notifier() = default;

  /// This callback is invoked on each outgoing UL DCCH PDU.
  virtual void on_ul_dcch_pdu(byte_buffer pdu) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
