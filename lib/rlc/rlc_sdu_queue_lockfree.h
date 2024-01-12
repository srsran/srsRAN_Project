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

#include "rlc_bearer_logger.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/rlc/rlc_tx.h"
#include "fmt/format.h"
#include <cstdint>

namespace srsran {

/// This class will hold RLC SDUs from upper layers.
/// It provides methods for thread-safe read/writing of SDUs
/// and discarding SDUs, if requested by the higher layers.
class rlc_sdu_queue_lockfree
{
public:
  explicit rlc_sdu_queue_lockfree(uint16_t capacity_, rlc_bearer_logger& logger_) :
    logger(logger_), capacity(capacity_), queue(capacity_)
  {
    sdu_states = new std::atomic<uint32_t>[capacity];
    sdu_sizes  = new std::atomic<size_t>[capacity];

    for (uint16_t i = 0; i < capacity; i++) {
      sdu_states[i].store(STATE_FREE, std::memory_order_relaxed);
    }
  }

  ~rlc_sdu_queue_lockfree()
  {
    delete[] sdu_sizes;
    delete[] sdu_states;
  }

  bool write(rlc_sdu sdu)
  {
    // if we have a PDCP SN, first check the slot is available
    optional<uint32_t> pdcp_sn  = sdu.pdcp_sn;
    const size_t       sdu_size = sdu.buf.length();
    if (pdcp_sn.has_value()) {
      const uint32_t pdcp_sn_value = sdu.pdcp_sn.value();
      uint32_t       slot_state    = sdu_states[pdcp_sn_value % capacity].load(std::memory_order_relaxed);
      if (slot_state != STATE_FREE) {
        logger.log_warning("Could not write to queue - slot not free. pdcp_sn={}", pdcp_sn_value);
        return false;
      }

      // slot is free, we can safely store the SDU size
      sdu_sizes[pdcp_sn_value % capacity].store(sdu_size, std::memory_order_relaxed);

      // reserve slot by writing the PDCP SN into it
      sdu_states[pdcp_sn_value % capacity].store(pdcp_sn_value, std::memory_order_release);
    }

    // push to queue
    bool pushed = queue.try_push(std::move(sdu));
    if (not pushed) {
      logger.log_warning("Could not write to queue - queue is full. pdcp_sn={}", pdcp_sn);
      // if we have a PDCP SN, release the slot that we just reserved
      if (pdcp_sn.has_value()) {
        sdu_states[pdcp_sn.value() % capacity].store(STATE_FREE);
      }
      return false;
    }

    // update totals
    n_sdus.fetch_add(1, std::memory_order_relaxed);
    n_bytes.fetch_add(sdu_size, std::memory_order_relaxed);
    return true;
  }

  bool discard(uint32_t pdcp_sn)
  {
    uint32_t expected_state = pdcp_sn;
    uint32_t desired_state  = STATE_DISCARDED;
    bool     discarded      = sdu_states[pdcp_sn % capacity].compare_exchange_strong(
        expected_state, desired_state, std::memory_order_relaxed);
    if (not discarded) {
      switch (expected_state) {
        case STATE_DISCARDED:
          logger.log_debug("Discard ignored for pdcp_sn={}. SDU already discarded", pdcp_sn);
          break;
        case STATE_FREE:
          logger.log_debug("Discard ignored for pdcp_sn={}. SDU is not present", pdcp_sn);
          break;
        default:
          logger.log_debug("Discard ignored for pdcp_sn={}. Slot occupied by pdcp_sn={}", pdcp_sn, expected_state);
          break;
      }
      return false;
    }

    // update totals
    n_sdus.fetch_sub(1, std::memory_order_relaxed);
    n_bytes.fetch_sub(sdu_sizes[pdcp_sn % capacity].load(std::memory_order_relaxed), std::memory_order_relaxed);
    return true;
  }

  bool read(rlc_sdu& sdu)
  {
    bool discard_successful = true;
    do {
      // first try to pop front (SDU can still get discarded from upper layers)
      bool popped = queue.try_pop(sdu);
      if (not popped) {
        // queue is empty
        return false;
      }

      if (sdu.pdcp_sn.has_value()) {
        // if we have a PDCP SN, try to discard it to check if it has been discarded before
        const uint32_t pdcp_sn_value = sdu.pdcp_sn.value();
        discard_successful           = discard(pdcp_sn_value);
        // release the slot
        sdu_states[pdcp_sn_value % capacity].store(STATE_FREE, std::memory_order_relaxed);
      } else {
        // without PDCP SN, proceed as if discarded
        discard_successful = true;

        // update totals (only for SDUs without PDCP SN which cannot be discarded)
        n_sdus.fetch_sub(1, std::memory_order_relaxed);
        n_bytes.fetch_sub(sdu.buf.length(), std::memory_order_relaxed);
      }
      // try again if discard failed, i.e. the SDU was already discarded before
    } while (not discard_successful);
    return true;
  }

  uint32_t size_sdus() const { return n_sdus.load(std::memory_order_relaxed); }

  uint32_t size_bytes() const { return n_bytes.load(std::memory_order_relaxed); }

  // bool front_size_bytes(uint32_t& size) { return 0; }

  bool is_empty() { return queue.empty(); }

  bool is_full() { return queue.size() >= capacity; }

private:
  static constexpr uint32_t STATE_FREE      = 0xffffffff;
  static constexpr uint32_t STATE_DISCARDED = 0xfffffffe;

  rlc_bearer_logger& logger;

  uint16_t              capacity = 256;
  std::atomic<uint32_t> n_bytes  = {0};
  std::atomic<uint32_t> n_sdus   = {0};

  concurrent_queue<rlc_sdu, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking> queue;

  std::atomic<uint32_t>* sdu_states;
  std::atomic<size_t>*   sdu_sizes;
};

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::rlc_sdu_queue_lockfree> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_sdu_queue_lockfree& q, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "queued_sdus={} queued_bytes={}", q.size_sdus(), q.size_bytes());
  }
};
} // namespace fmt
