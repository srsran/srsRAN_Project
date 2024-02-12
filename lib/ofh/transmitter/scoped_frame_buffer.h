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

#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/support/error_handling.h"

namespace srsran {
namespace ofh {

/// Scoped helper class that retrieves an Ethernet frame buffer from a buffer pool and marks it as ready to send upon
/// destruction.
class scoped_frame_buffer
{
  ether::eth_frame_pool&          frame_pool;
  const ether::frame_pool_context context;
  const span<ether::frame_buffer> frames;

public:
  /// On construction, acquire Ethernet frame buffers for the given slot, symbol and Open Fronthaul type.
  scoped_frame_buffer(ether::eth_frame_pool& frame_pool_,
                      slot_symbol_point      symbol_point,
                      message_type           type,
                      data_direction         direction) :
    frame_pool(frame_pool_), context({{type, direction}, symbol_point}), frames(frame_pool.get_frame_buffers(context))
  {
  }

  /// Returns the frame retrieved from the pool.
  ether::frame_buffer& get_next_frame()
  {
    for (auto& frame : frames) {
      if (frame.empty()) {
        return frame;
      }
    }

    srsran_terminate("No empty Ethernet frame available in slot '{}' symbol '{}'\n",
                     context.symbol_point.get_slot(),
                     context.symbol_point.get_symbol_index());
  }

  bool empty() const { return frames.empty(); }

  /// Destructor marks the acquired buffers as ready to be sent.
  ~scoped_frame_buffer()
  {
    if (!frames.empty()) {
      frame_pool.push_frame_buffers(context, frames);
    }
  }
};

} // namespace ofh
} // namespace srsran
