/*
 *
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer_slice_chain.h"

namespace srsran {
namespace srs_cu_up {

/// \brief This interface represents the data exit point of the receiving side of a F1-U bearer of the CU-UP.
/// The F1-U bearer will use this notifier to pass NR-U SDUs (e.g. PDCP PDUs/RLC SDUs) to upper layer (e.g. PDCP).
class f1u_rx_sdu_notifier
{
public:
  virtual ~f1u_rx_sdu_notifier() = default;

  virtual void on_new_sdu(byte_buffer_slice_chain sdu) = 0;
};

} // namespace srs_cu_up
} // namespace srsran
