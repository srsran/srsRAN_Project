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

#include "f1ap_ue_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/slotted_array.h"

namespace srsran {
namespace srs_cu_cp {

class f1ap_ul_ccch_notifier;
class f1ap_ul_dcch_notifier;

/// Handles the reception of an UL RRC PDU initiated by an UL RRC message transfer procedure.
class f1c_ul_bearer_handler
{
public:
  virtual ~f1c_ul_bearer_handler() = default;

  /// Handle the reception of an UL RRC PDU.
  virtual void handle_ul_rrc_message(byte_buffer pdu) = 0;
};

/// Manages the state of the uplink path for the Signaling Radio Bearers (SRBs) of an individual UE.
class ue_ul_bearer_manager
{
public:
  /// Activate SRB0.
  void activate_srb0(f1ap_ul_ccch_notifier& f1ap_srb0_notifier);

  /// Activate SRB1.
  void activate_srb1(f1ap_ul_dcch_notifier& f1ap_srb1_notifier);

  /// Activate SRB2.
  void activate_srb2(f1ap_ul_dcch_notifier& f1ap_srb2_notifier);

  /// Returns a pointer to the SRB bearer handler, or nullptr if the SRB has not been activated.
  f1c_ul_bearer_handler* get_srb(srb_id_t srb_id) { return f1c_ul_bearers[srb_id_to_uint(srb_id)].get(); }

  /// Returns a pointer to the SRB bearer handler, or nullptr if the SRB has not been activated.
  const f1c_ul_bearer_handler* get_srb(srb_id_t srb_id) const { return f1c_ul_bearers[srb_id_to_uint(srb_id)].get(); }

private:
  slotted_array<std::unique_ptr<f1c_ul_bearer_handler>, MAX_NOF_SRBS> f1c_ul_bearers;
};

} // namespace srs_cu_cp
} // namespace srsran
