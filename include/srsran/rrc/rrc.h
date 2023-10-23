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

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/lcid.h"

namespace srsran {

namespace srs_cu_cp {

enum ue_context_release_cause : uint16_t {
  radio_network = 0,
  transport     = 1,
  protocol      = 2,
  misc          = 3,
  choice_ext    = 4,
  nulltype      = 5
};

/// Interface to inform about changes to the AMF state.
class rrc_amf_connection_handler
{
public:
  virtual ~rrc_amf_connection_handler() = default;

  /// \brief Handle a successful AMF connection.
  virtual void handle_amf_connection() = 0;
  /// \brief Handle a dropped AMF connection.
  virtual void handle_amf_connection_drop() = 0;
};

/// This interface represents the data entry point for the RRC receiving PDUs on the UL-CCCH logical channel.
/// The lower-layers will use this class to pass PDUs into the RRC.
class rrc_ul_ccch_pdu_handler
{
public:
  virtual ~rrc_ul_ccch_pdu_handler() = default;

  /// Handle the incoming PDU on the UL-CCCH logical channel.
  virtual void handle_ul_ccch_pdu(byte_buffer pdu) = 0;
};

/// This interface represents the data entry point for the RRC receiving PDUs on the UL-DCCH logical channel.
/// The lower-layers will use this class to pass PDUs into the RRC.
class rrc_ul_dcch_pdu_handler
{
public:
  virtual ~rrc_ul_dcch_pdu_handler() = default;

  /// Handle the incoming SRB PDCP PDU on the UL-DCCH logical channel.
  virtual void handle_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) = 0;
};

/// This interface represents the data entry point for the RRC receiving NAS PDUs.
/// The higher-layers will use this class to pass PDUs into the RRC.
class rrc_dl_nas_message_handler
{
public:
  virtual ~rrc_dl_nas_message_handler() = default;

  /// \brief Handle the received Downlink NAS Transport message.
  /// \param[in] nas_pdu The received NAS PDU.
  virtual void handle_dl_nas_transport_message(byte_buffer nas_pdu) = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
