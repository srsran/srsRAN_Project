/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {
namespace srs_du {

/// \brief This interface represents the data exit point of the receiving side of a F1-C bearer of the DU.
/// The F1AP will push F1-C SDUs (e.g. PDCP PDUs/RLC SDUs) to the lower layer (e.g. RLC) using this interface.
class f1c_rx_sdu_notifier
{
public:
  virtual ~f1c_rx_sdu_notifier() = default;

  virtual void on_new_sdu(byte_buffer pdu) = 0;
};

} // namespace srs_du
} // namespace srsgnb