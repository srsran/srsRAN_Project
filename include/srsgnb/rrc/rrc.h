/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

namespace srs_cu_cp {

enum ue_context_release_cause : uint16_t {
  radio_network = 0,
  transport     = 1,
  protocol      = 2,
  misc          = 3,
  choice_ext    = 4,
  nulltype      = 5
};

struct ue_context_release_command_message {
  ue_index_t               ue_index;
  ue_context_release_cause cause;
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
  virtual void handle_ul_ccch_pdu(byte_buffer_slice pdu) = 0;
};

/// This interface represents the data entry point for the RRC receiving PDUs on the UL-DCCH logical channel.
/// The lower-layers will use this class to pass PDUs into the RRC.
class rrc_ul_dcch_pdu_handler
{
public:
  virtual ~rrc_ul_dcch_pdu_handler() = default;

  /// Handle the incoming PDU on the UL-DCCH logical channel.
  virtual void handle_ul_dcch_pdu(byte_buffer_slice pdu) = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb
