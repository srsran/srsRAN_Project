/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RRC_H
#define SRSGNB_RRC_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

namespace srs_cu_cp {

/// This interface represents the data entry point for the RRC receiving PDUs on the UL-CCCH logical channel.
/// The lower-layers will use this class to pass PDUs into the RRC.
class rrc_ul_ccch_pdu_handler
{
public:
  virtual ~rrc_ul_ccch_pdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle_ul_ccch_pdu(shared_byte_buffer_view pdu) = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_RRC_H
