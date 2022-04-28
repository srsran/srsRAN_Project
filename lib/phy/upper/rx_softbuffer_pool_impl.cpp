
#include "rx_softbuffer_pool_impl.h"

using namespace srsgnb;
rx_softbuffer* rx_softbuffer_pool_impl::reserve_softbuffer(const slot_point&               slot,
                                                           const rx_softbuffer_identifier& id,
                                                           unsigned int                    nof_codeblocks)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Look for the same identifier.
  for (rx_softbuffer_impl& buffer : buffers) {
    if (buffer.match_id(id)) {
      slot_point expire_slot = slot + expire_timeout_slots;
      buffer.reserve(id, expire_slot, nof_codeblocks);
      return &buffer;
    }
  }

  // If the same identifier was not found, select the first available.
  for (rx_softbuffer_impl& buffer : buffers) {
    if (!buffer.is_reserved()) {
      slot_point expire_slot = slot + expire_timeout_slots;
      buffer.reserve(id, expire_slot, nof_codeblocks);
      return &buffer;
    }
  }

  // If no available buffer is found...
  return nullptr;
}

void rx_softbuffer_pool_impl::free_softbuffer(const rx_softbuffer_identifier& id)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Search for a buffer that matches the id.
  for (rx_softbuffer_impl& buffer : buffers) {
    if (buffer.match_id(id)) {
      // Free and early return.
      buffer.free();
      return;
    }
  }
}

void rx_softbuffer_pool_impl::run_slot(const slot_point& slot)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Run slot for each buffer.
  for (rx_softbuffer_impl& buffer : buffers) {
    buffer.run_slot(slot);
  }
}
