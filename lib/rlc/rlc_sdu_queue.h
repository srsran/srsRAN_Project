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

#include "srsgnb/adt/blocking_queue.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "fmt/format.h"
#include <cstdint>
#include <functional>
#include <list>

namespace srsran {

/// This class will hold RLC SDUs from upper layers.
/// It provides methods for thread-safe read/writing of SDUs
/// and discarding SDUs, if requested by the higher layers.
class rlc_sdu_queue
{
public:
  explicit rlc_sdu_queue(uint16_t capacity_ = 256) :
    capacity(capacity_), queue(capacity_, push_callback(unread_bytes, n_sdus), pop_callback(unread_bytes, n_sdus))
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
    // The SDU byte_buffer might be empty, due to having been discarded.
    // We try to read the next one if that is the case.
    optional<rlc_sdu> popped_elem =
        queue.pop_and_discard_until([](const rlc_sdu& elem) { return not elem.buf.empty(); });
    if (popped_elem.has_value()) {
      sdu = std::move(*popped_elem);
      return true;
    }
    return false;
  }

  uint32_t size_sdus() const { return n_sdus; }

  uint32_t size_bytes() const { return unread_bytes; }

  bool front_size_bytes(uint32_t& size)
  {
    return queue.try_call_on_front([&size](const rlc_sdu& sdu) { size = sdu.buf.length(); });
  }

  bool is_empty() { return queue.empty(); }

  bool is_full() { return queue.size() >= capacity; }

  // Marks a packet as discarded by calling `clear` on it.
  bool discard(uint32_t pdcp_sn)
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

  blocking_queue<rlc_sdu, push_callback, pop_callback> queue;
};

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::rlc_sdu_queue> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_sdu_queue& q, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "queued_sdus={} queued_bytes={}", q.size_sdus(), q.size_bytes());
  }
};
} // namespace fmt
