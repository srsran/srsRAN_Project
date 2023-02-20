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

#include "srsran/mac/cell_configuration.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/memory_pool/ring_buffer_pool.h"

namespace srsran {

/// Encodes RAR PDUs based on RAR information provided by the scheduler.
class rar_pdu_assembler
{
public:
  explicit rar_pdu_assembler(ticking_ring_buffer_pool& pdu_pool);

  span<const uint8_t> encode_rar_pdu(const rar_information& rar);

private:
  ticking_ring_buffer_pool& pdu_pool;
};

} // namespace srsran
