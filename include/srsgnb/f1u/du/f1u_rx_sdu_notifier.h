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

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {
namespace srs_du {

/// \brief This interface represents the NR-U exit point of a F1-U entity.
/// The F1-U entity will use this notifier to pass NR-U PDUs to upper-layer transport (GTP-U).
class f1u_rx_sdu_notifier
{
public:
  virtual ~f1u_rx_sdu_notifier() = default;

  virtual void on_new_rx_sdu(byte_buffer buf) = 0;
};

} // namespace srs_du
} // namespace srsgnb