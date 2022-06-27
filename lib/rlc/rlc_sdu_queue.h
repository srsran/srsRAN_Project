/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_SDU_QUEUE_H
#define SRSGNB_RLC_SDU_QUEUE_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/circular_buffer.h"
#include "srsgnb/rlc/rlc.h"
#include <cstdint>
#include <functional>
#include <list>

namespace srsgnb {

/// This class will hold RLC SDUs from upper layers.
/// It provides methods for thread-safe read/writing of SDUs
/// and discarding SDUs, if requested by the higher layers.
class rlc_sdu_queue
{
public:
  explicit rlc_sdu_queue(uint16_t capacity = 256) :
    capacity(capacity), queue(capacity, push_callback(unread_bytes, n_sdus), pop_callback(unread_bytes, n_sdus))
  {
  }

  bool write(rlc_sdu sdu)
  {
    if (queue.size() >= capacity) {
      return false;
    }
    queue.push_blocking(std::move(sdu));
    return true;
  }

  bool read(rlc_sdu& sdu)
  {
    if (is_empty()) {
      return false;
    }

    // The SDU byte_buffer might be empty, do to having been discarded.
    // We try to read the next one if that is the case.
    do {
      sdu = queue.pop_blocking();
    } while (sdu.buf.empty() and not is_empty());

    if (sdu.buf.empty()) {
      return false;
    }
    return true;
  }

  uint32_t size_sdus() const { return n_sdus; }

  uint32_t size_bytes() const { return unread_bytes; }

  bool is_empty() { return queue.empty(); }

  bool is_full() { return queue.size() >= capacity; }

  // Marks a packet as discarded by calling `clear` on it.
  bool discard_sn(uint32_t pdcp_sn)
  {
    bool discarded = queue.apply_first([&pdcp_sn, this](rlc_sdu& sdu) {
      if (!sdu.buf.empty() && sdu.pdcp_sn == pdcp_sn) {
        queue.pop_func(sdu);
        sdu.buf.clear();
        return true;
      }
      return false;
    });
    return discarded;
  }

private:
  struct push_callback {
    explicit push_callback(std::atomic<uint32_t>& unread_bytes_, std::atomic<uint32_t>& n_sdus_) :
      unread_bytes(unread_bytes_), n_sdus(n_sdus_)
    {
    }
    void operator()(const rlc_sdu& sdu)
    {
      unread_bytes.fetch_add(sdu.buf.length(), std::memory_order_relaxed);
      n_sdus.fetch_add(1, std::memory_order_relaxed);
    }
    std::atomic<uint32_t>& unread_bytes;
    std::atomic<uint32_t>& n_sdus;
  };
  struct pop_callback {
    explicit pop_callback(std::atomic<uint32_t>& unread_bytes_, std::atomic<uint32_t>& n_sdus_) :
      unread_bytes(unread_bytes_), n_sdus(n_sdus_)
    {
    }
    void operator()(const rlc_sdu& sdu)
    {
      if (sdu.buf.empty()) {
        return;
      }
      // non-atomic update of both state variables
      unread_bytes.fetch_sub(std::min((uint32_t)sdu.buf.length(), unread_bytes.load(std::memory_order_relaxed)),
                             std::memory_order_relaxed);
      n_sdus.store(std::max(0, (int32_t)(n_sdus.load(std::memory_order_relaxed)) - 1), std::memory_order_relaxed);
    }
    std::atomic<uint32_t>& unread_bytes;
    std::atomic<uint32_t>& n_sdus;
  };
  uint16_t              capacity     = 256;
  std::atomic<uint32_t> unread_bytes = {0};
  std::atomic<uint32_t> n_sdus       = {0};

  dyn_blocking_queue<rlc_sdu, push_callback, pop_callback> queue;
};

} // namespace srsgnb
#endif
