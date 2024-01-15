/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                      slot_point             slot_,
                      unsigned               symbol_,
                      message_type           type_,
                      data_direction         direction_) :
    frame_pool(frame_pool_), context(slot_, symbol_, type_, direction_), frames(frame_pool.get_frame_buffers(context))
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

    srsran_terminate("No empty Ethernet frame available in slot={}\n", context.slot);
  }

  bool empty() const { return frames.empty(); }

  /// Destructor marks the acquired buffers as ready to be sent.
  ~scoped_frame_buffer()
  {
    if (!frames.empty()) {
      frame_pool.eth_frames_ready(context, frames);
    }
  }
};

} // namespace ofh
} // namespace srsran
