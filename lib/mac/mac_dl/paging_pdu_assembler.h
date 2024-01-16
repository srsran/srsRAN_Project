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

#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/memory_pool/ring_buffer_pool.h"

namespace srsran {

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
