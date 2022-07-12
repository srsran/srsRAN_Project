/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_CU_CP_SRB_PDU_NOTIFIER_H
#define SRSGNB_CU_CP_SRB_PDU_NOTIFIER_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// Interface to notify the reception of an new SRB PDU.
class srb_pdu_notifier
{
public:
  virtual ~srb_pdu_notifier() = default;

  /// This callback is invoked on each received PDU.
  virtual void on_new_pdu(byte_buffer_slice pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_CU_CP_SRB_PDU_NOTIFIER_H
