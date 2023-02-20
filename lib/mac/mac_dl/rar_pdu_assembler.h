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

#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include "srsgnb/support/memory_pool/ring_buffer_pool.h"

namespace srsgnb {

/// Encodes RAR PDUs based on RAR information provided by the scheduler.
class rar_pdu_assembler
{
public:
  explicit rar_pdu_assembler(ticking_ring_buffer_pool& pdu_pool);

  span<const uint8_t> encode_rar_pdu(const rar_information& rar);

private:
  ticking_ring_buffer_pool& pdu_pool;
};

} // namespace srsgnb
