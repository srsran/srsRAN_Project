/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1u/common/nru_message.h"

namespace srsgnb {
namespace srs_cu_up {

/// \brief This interface represents the NR-U exit point of the F1-U bearer of the CU-UP
/// through which it passes NR-U PDUs to lower layer (e.g. GTP-U) towards the DU.
class f1u_tx_pdu_notifier
{
public:
  virtual ~f1u_tx_pdu_notifier() = default;

  virtual void on_new_pdu(nru_dl_message msg) = 0;
};

} // namespace srs_cu_up
} // namespace srsgnb
