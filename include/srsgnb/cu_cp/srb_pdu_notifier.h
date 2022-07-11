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
#include "srsgnb/asn1/asn1_utils.h"

namespace srsgnb {

/// Interface to notify the reception of an new SRB PDU.
class srb_pdu_notifier
{
public:
  virtual ~srb_pdu_notifier() = default;

  /// This callback is invoked on each received PDU.
  virtual void on_new_pdu(byte_buffer_slice pdu) = 0;

  /// This callback is invoked on each received RRC message.
  virtual void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) = 0;
};

} // namespace srsgnb
