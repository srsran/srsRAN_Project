/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
