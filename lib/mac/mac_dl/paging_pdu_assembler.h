/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/memory_pool/ring_buffer_pool.h"

namespace srsran {

struct dl_paging_allocation;

/// Encodes Paging PDUs based on Paging information provided by the scheduler.
class paging_pdu_assembler
{
public:
  paging_pdu_assembler(ticking_ring_buffer_pool& pdu_pool_) : pdu_pool(pdu_pool_) {}

  /// \brief Encode Paging MAC PDU from Paging grant information.
  /// \param[in] pg Paging grant information.
  /// \return Encoded Paging MAC PDU.
  span<const uint8_t> encode_paging_pdu(const dl_paging_allocation& pg);

private:
  /// Buffer pool holding Paging PDUs.
  ticking_ring_buffer_pool& pdu_pool;
};

} // namespace srsran
