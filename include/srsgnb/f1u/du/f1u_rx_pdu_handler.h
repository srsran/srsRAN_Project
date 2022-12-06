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

#include "srsgnb/adt/byte_buffer_slice_chain.h"

namespace srsgnb {
namespace srs_du {

/// \brief This interface represents the data entry point of the receiving side of a F1-U bearer.
/// The lower-layers will use this class to pass PDUs into the F1-U.
class f1u_rx_pdu_handler
{
public:
  virtual ~f1u_rx_pdu_handler() = default;

  virtual void handle_pdu(byte_buffer_slice_chain pdu) = 0;
};

} // namespace srs_du
} // namespace srsgnb