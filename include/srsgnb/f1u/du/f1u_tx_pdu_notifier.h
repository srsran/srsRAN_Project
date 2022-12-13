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
namespace srs_du {

/// \brief This interface represents the NR-U exit point of a F1-U bearer of the DU.
/// The F1-U bearer will use this notifier to pass NR-U PDUs to upper layer (e.g. GTP-U) towards CU-UP.
class f1u_tx_pdu_notifier
{
public:
  virtual ~f1u_tx_pdu_notifier() = default;

  virtual void on_new_pdu(nru_ul_message msg) = 0;
};

} // namespace srs_du
} // namespace srsgnb
