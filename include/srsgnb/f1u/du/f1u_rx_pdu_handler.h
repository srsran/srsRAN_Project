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

/// \brief This interface represents the NR-U entry point of the receiving side of a F1-U bearer of the DU.
/// The upper layer (e.g. GTP-U) will use it to pass NR-U PDUs (from the CU-UP) into the F1-U bearer of the DU.
class f1u_rx_pdu_handler
{
public:
  virtual ~f1u_rx_pdu_handler() = default;

  virtual void handle_pdu(nru_dl_message msg) = 0;
};

} // namespace srs_du
} // namespace srsgnb
