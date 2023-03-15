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

#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

/// Encodes Paging PDUs based on Paging information provided by the scheduler.
class paging_pdu_assembler
{
public:
  paging_pdu_assembler() = default;

  /// \brief Encode Paging MAC PDU from Paging grant information.
  /// \param[in] pg Paging grant information.
  /// \return Encoded Paging MAC PDU.
  static span<const uint8_t> encode_paging_pdu(const dl_paging_allocation& pg);
};

} // namespace srsran
