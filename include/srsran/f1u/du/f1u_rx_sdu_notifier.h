/*
 *
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"

namespace srsran {
namespace srs_du {

/// \brief This interface represents the data exit point of the receiving side of a F1-U bearer of the DU.
/// The F1-U will push NR-U SDUs (e.g. PDCP PDUs/RLC SDUs) to the lower layer (e.g. RLC) using this interface.
/// The F1-U will also inform the lower layer (e.g. RLC) of SDUs (e.g. PDCP PDUs) to be discarded.
class f1u_rx_sdu_notifier
{
public:
  virtual ~f1u_rx_sdu_notifier() = default;

  virtual void on_new_sdu(byte_buffer sdu, bool is_retx) = 0;
  virtual void on_discard_sdu(uint32_t pdcp_sn)          = 0;
};

} // namespace srs_du
} // namespace srsran
