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

#include "rlc_bearer_logger.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/rlc/rlc_tx.h"

namespace srsran {

/// \brief Lockfree RLC SDU queue
///
/// This class is used as FIFO buffer for RLC SDUs from upper layers that shall be transmitted by the RLC Tx entity.
/// It implements SPSC (single producer single consumer) semantics, i.e. allows concurrent access by one upper-layer
/// thread and one lower-layer thread. Internally, this class wraps a lock-free SPSC queue to store the actual SDUs and
/// involves additional bookkeeping to support discard of SDUs and to track the amount of buffered SDUs and bytes.
///
/// From the perspective of the upper-layer thread this class provides methods to write RLC SDUs (with or without PDCP
/// SN) and to discard SDUs by their PDCP SN. SDUs without PDCP SN cannot be discarded.
/// SDUs that are marked as discarded remain in the internal queue until they are popped (and dropped) via \c read.
/// Writing to the queue fails if the internal queue is either full or the queue already contains an SDU (whether valid
/// or marked as discarded) with the same value for [PDCP_SN mod capacity].
///
/// From the perspective of the lower-layer thread it provides methods to read RLC SDUs and to query the total number of
/// buffered SDUs and bytes.
/// SDUs are read in the same order as they were written into the queue. There is no reordering by PDCP SN.
class rlc_sdu_queue_lockfree
{
public:
  explicit rlc_sdu_queue_lockfree(uint16_t capacity_, rlc_bearer_logger& logger_) : logger(logger_), capacity(capacity_)
  {
    sdu_states = std::make_unique<std::atomic<uint32_t>[]>(capacity);
    sdu_sizes  = std::make_unique<std::atomic<size_t>[]>(capacity);

    for (uint16_t i = 0; i < capacity; i++) {
      sdu_states[i].store(STATE_FREE, std::memory_order_relaxed);
    }

    queue = std::make_unique<
        concurrent_queue<rlc_sdu, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>>(
        capacity);
  }

  /// \brief Writes an RLC SDU (with optional PDCP SN) to the queue, if possible.
  ///
  /// This function may be called by the upper-layer thread.
  ///
  /// The write fails (returns false) in the following cases:
  /// - The internal queue is full.
  /// - Another SDU with same value of [PDCP_SN mod capacity] exists (either valid or discarded) in the queue.
  ///
  /// \param sdu The RLC SDU that shall be written.
  /// \return True if the RLC SDU was successfully written to the queue, otherwise false.
  bool write(rlc_sdu sdu)
  {
    // if the SDU has a PDCP SN, first check the slot is available
    optional<uint32_t> pdcp_sn  = sdu.pdcp_sn;
    const size_t       sdu_size = sdu.buf.length();
    if (pdcp_sn.has_value()) {
      const uint32_t pdcp_sn_value = sdu.pdcp_sn.value();
      // load slot state (memory_order_acquire ensures sdu_size is written after this)
      uint32_t slot_state = sdu_states[pdcp_sn_value % capacity].load(std::memory_order_acquire);
      if (slot_state != STATE_FREE) {
        logger.log_debug("SDU queue failed to enqueue pdcp_sn={}. Slot holds pdcp_sn={}", pdcp_sn_value, slot_state);
        return false;
      }

      // slot is free, we can safely store the SDU size
      sdu_sizes[pdcp_sn_value % capacity].store(sdu_size, std::memory_order_relaxed);

      // allocate slot by writing the PDCP SN into it (memory_order_relaxed, don't care about order because of "release"
      // semantics of the push to queue)
      sdu_states[pdcp_sn_value % capacity].store(pdcp_sn_value, std::memory_order_relaxed);
    }

    // push SDU to queue
    bool pushed = queue->try_push(std::move(sdu));
    if (not pushed) {
      logger.log_debug("SDU queue failed to enqueue pdcp_sn={}. Queue is full", pdcp_sn);
      // if the SDU has a PDCP SN, release the slot (memory_order_relaxed, don't care about value of sdu_size)
      if (pdcp_sn.has_value()) {
        sdu_states[pdcp_sn.value() % capacity].store(STATE_FREE, std::memory_order_relaxed);
      }
      return false;
    }

    // update totals
    n_sdus.fetch_add(1, std::memory_order_relaxed);
    n_bytes.fetch_add(sdu_size, std::memory_order_relaxed);
    return true;
  }

  /// \brief Marks an RLC SDU as "discarded", if possible.
  ///
  /// This function may be called by the upper-layer thread.
  ///
  /// The function fails (returns false) in the following cases:
  /// - The SDU with the PDCP SN is already marked as discarded.
  /// - The SDU with the PDCP SN is not in the queue.
  ///
  /// In case of success, the number of SDUs/bytes will be updated immediately.
  /// However, the actual SDU remains in the internal queue until it is popped and finally discarded via \c read.
  /// Meanwhile, no further SDU with the same value of [PDCP_SN mod capacity] can be added to the queue.
  ///
  /// \param pdcp_sn The PDCP SN of the SDU that shall be discarded.
  /// \return True if the RLC SDU was successfully discarded, otherwise false.
  bool try_discard(uint32_t pdcp_sn)
  {
    uint32_t expected_state = pdcp_sn;
    uint32_t desired_state  = STATE_DISCARDED;
    // set slot state as "discarded" only if it holds the PDCP SN; otherwise load the existing state
    // - on success: memory_order_acquire ensures sdu_size is up to date
    // - on failure: memory_order_relaxed, don't care about value of sdu_size
    bool success = sdu_states[pdcp_sn % capacity].compare_exchange_strong(
        expected_state, desired_state, std::memory_order_acquire, std::memory_order_relaxed);
    uint32_t sdu_size = sdu_sizes[pdcp_sn % capacity].load(std::memory_order_relaxed);

    if (not success) {
      switch (expected_state) {
        case STATE_DISCARDED:
          logger.log_debug("SDU queue cannot discard pdcp_sn={}. Slot is already discarded", pdcp_sn);
          break;
        case STATE_FREE:
          logger.log_debug("SDU queue cannot discard pdcp_sn={}. Slot is already free", pdcp_sn);
          break;
        default:
          logger.log_debug("SDU queue cannot discard pdcp_sn={}. Slot holds pdcp_sn={}", pdcp_sn, expected_state);
          break;
      }
      return false;
    }

    // update totals
    n_sdus.fetch_sub(1, std::memory_order_relaxed);
    n_bytes.fetch_sub(sdu_size, std::memory_order_relaxed);
    return true;
  }

  /// \brief Reads an RLC SDU (with optional PDCP SN) from the queue, if possible.
  ///
  /// This function may be called by the lower-layer thread.
  ///
  /// The read fails (returns false) in the following cases:
  /// - The internal queue only contains SDUs that are marked as discarded.
  /// - The internal queue is empty.
  ///
  /// Each call of this function pops and drops SDUs that are marked as discarded from the internal queue in a loop
  /// until a valid SDU is popped (return true) or the queue is empty (returns false).
  ///
  /// \param sdu Reference to a \c rlc_sdu object that will be filled with the read RLC SDU.
  /// \return True if an RLC SDU was successfully read from the queue, otherwise false.
  bool read(rlc_sdu& sdu)
  {
    bool sdu_is_valid = true;
    do {
      // first try to pop front (SDU can still get discarded from upper layers)
      bool popped = queue->try_pop(sdu);
      if (not popped) {
        // queue is empty
        return false;
      }

      if (sdu.pdcp_sn.has_value()) {
        // Check if the SDU is still valid (i.e. the PDCP SN was not already discarded) and release the slot
        const uint32_t pdcp_sn = sdu.pdcp_sn.value();
        sdu_is_valid           = check_and_release(pdcp_sn); // this also updates totals
      } else {
        // SDUs without PDCP SN are alway valid as they can't be discarded
        sdu_is_valid = true;

        // update totals
        n_sdus.fetch_sub(1, std::memory_order_relaxed);
        n_bytes.fetch_sub(sdu.buf.length(), std::memory_order_relaxed);
      }
      // try again if SDU is not valid
    } while (not sdu_is_valid);
    return true;
  }

  /// \brief Reads the number of buffered SDUs that are not marked as discarded.
  ///
  /// This function may be called by any thread.
  ///
  /// \return The number of buffered SDUs that are not marked as discarded.
  uint32_t size_sdus() const { return n_sdus.load(std::memory_order_relaxed); }

  /// \brief Reads the number of buffered SDU bytes that are not marked as discarded.
  ///
  /// This function may be called by any thread.
  ///
  /// \return The number of buffered SDU bytes that are not marked as discarded.
  uint32_t size_bytes() const { return n_bytes.load(std::memory_order_relaxed); }

  /// \brief Container for return value of \c get_state function.
  struct state {
    uint32_t n_sdus;  ///< Number of buffered SDUs that are not marked as discarded.
    uint32_t n_bytes; ///< Number of buffered bytes that are not marked as discarded.
  };

  /// \brief Reads the state of the queue, i.e. number of buffered SDUs and bytes that are not marked as discarded.
  ///
  /// This function may be called by any thread.
  ///
  /// \return Current state of the queue.
  state get_state() const { return {size_sdus(), size_bytes()}; }

  /// \brief Checks if the internal queue is empty.
  ///
  /// This function may be called by any thread.
  ///
  /// \return True if the internal queue is empty, otherwise false.
  bool is_empty() { return queue->empty(); }

  /// \brief Checks if the internal queue is full.
  ///
  /// This function may be called by any thread.
  ///
  /// \return True if the internal queue is full, otherwise false.
  bool is_full() { return queue->size() >= capacity; }

private:
  /// \brief Checks if the RLC SDU with a PDCP SN is valid (i.e. not marked as discarded) and marks the respective slot
  /// as free.
  ///
  /// This private function may be called by \c read by the lower-layer thread.
  ///
  /// The function fails (returns false) in the following cases:
  /// - The SDU with the PDCP SN is already marked as discarded.
  /// - The SDU with the PDCP SN is not in the queue.
  ///
  /// The number of SDUs/bytes will be updated unless the SDU is marked as discarded (in that case it was already
  /// updated by \c try_discard).
  ///
  /// \param pdcp_sn The PDCP SN of the SDU that shall be checked and released.
  /// \return True if the RLC SDU with given PDCP SN is valid, otherwise false.
  bool check_and_release(uint32_t pdcp_sn)
  {
    uint32_t sdu_size = sdu_sizes[pdcp_sn % capacity].load(std::memory_order_relaxed);
    // free the slot (memory_order_release ensures sdu_size is read before the state is set to free)
    uint32_t old_state = sdu_states[pdcp_sn % capacity].exchange(STATE_FREE, std::memory_order_release);

    bool sdu_is_valid;
    if (old_state == pdcp_sn) {
      sdu_is_valid = true;
      logger.log_debug("SDU queue released valid pdcp_sn={}", pdcp_sn);
    }
    if (old_state != pdcp_sn) {
      sdu_is_valid = false;
      switch (old_state) {
        case STATE_DISCARDED:
          logger.log_debug("SDU queue released discarded pdcp_sn={}", pdcp_sn);
          return false; // totals were already updated on discard
        case STATE_FREE:
          logger.log_error("SDU queue error: Invalid release of pdcp_sn={}. Slot was already free", pdcp_sn);
          break;
        default:
          logger.log_error("SDU queue error: Invalid release of pdcp_sn={}. Slot holds pdcp_sn={}", pdcp_sn, old_state);
          break;
      }
    }

    // update totals
    n_sdus.fetch_sub(1, std::memory_order_relaxed);
    n_bytes.fetch_sub(sdu_size, std::memory_order_relaxed);
    return sdu_is_valid;
  }

  static constexpr uint32_t STATE_FREE      = 0xffffffff; ///< Sentinel value to mark a slot as free.
  static constexpr uint32_t STATE_DISCARDED = 0xfffffffe; ///< Sentinel value to mark a slot as discarded.

  rlc_bearer_logger& logger;

  uint16_t              capacity;
  std::atomic<uint32_t> n_bytes = {0};
  std::atomic<uint32_t> n_sdus  = {0};

  std::unique_ptr<
      concurrent_queue<rlc_sdu, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>>
      queue;

  std::unique_ptr<std::atomic<uint32_t>[]> sdu_states;
  std::unique_ptr<std::atomic<size_t>[]>   sdu_sizes;
};

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::rlc_sdu_queue_lockfree::state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_sdu_queue_lockfree::state& state, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "queued_sdus={} queued_bytes={}", state.n_sdus, state.n_bytes);
  }
};

} // namespace fmt
