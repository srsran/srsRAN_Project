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

#include "rlc_am_pdu.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"
#include <cstdint>

namespace srsran {

struct rlc_tx_amd_retx {
  uint32_t sn     = INVALID_RLC_SN; ///< sequence number
  uint32_t so     = 0;              ///< offset to first byte of this ReTx within the associated SDU
  uint32_t length = 0;              ///< length of this ReTx

  /// \brief Implements a check whether the range of this retransmission object includes the given segment range
  /// \param segment_offset The segment offset to check
  /// \param segment_length Length of the segment to check
  /// \return true if the segment offset is covered by the retransmission object. Otherwise false
  bool includes(uint32_t segment_offset, uint32_t segment_length) const
  {
    return (segment_offset >= so) && (segment_offset + segment_length <= so + length);
  }
};

class rlc_retx_queue_state
{
  uint32_t retx_bytes        = 0; ///< Total amount of queued PDU payload bytes across all queued ReTx
  uint32_t n_retx_so_zero    = 0; ///< Number of queued ReTx with zero segment offset, i.e. need small PDU header
  uint32_t n_retx_so_nonzero = 0; ///< Number of queued ReTx with non-zero segment offset, i.e. need large PDU header

public:
  uint32_t get_retx_bytes() const { return retx_bytes; }
  uint32_t get_n_retx_so_zero() const { return n_retx_so_zero; }
  uint32_t get_n_retx_so_nonzero() const { return n_retx_so_nonzero; }

  void add(const rlc_tx_amd_retx& elem)
  {
    retx_bytes += elem.length;
    if (elem.so == 0) {
      n_retx_so_zero++;
    } else {
      n_retx_so_nonzero++;
    }
  }

  void subtract(const rlc_tx_amd_retx& elem)
  {
    if (retx_bytes >= elem.length) {
      retx_bytes -= elem.length;
    } else {
      retx_bytes = 0;
    }
    if (elem.so == 0) {
      if (n_retx_so_zero > 0) {
        n_retx_so_zero--;
      }
    } else {
      if (n_retx_so_nonzero > 0) {
        n_retx_so_nonzero--;
      }
    }
  }
};

struct rlc_retx_queue_item {
  bool            invalid = false; ///< Marks element as virtually removed "zombie" which can be skipped on pop
  rlc_tx_amd_retx retx    = {};    ///< The actual ReTx info
};

class rlc_retx_queue
{
  ring_buffer<rlc_retx_queue_item> queue;
  rlc_retx_queue_state             st;

public:
  rlc_retx_queue(size_t capacity) : queue(capacity) {}
  ~rlc_retx_queue() = default;

  /// \brief Tries to push a ReTx to the queue.
  /// \param retx The ReTx to be added.
  /// \return True if the ReTx was added successfully; False if the queue was full.
  bool try_push(const rlc_tx_amd_retx& retx)
  {
    rlc_retx_queue_item elem = {};
    elem.retx                = retx;
    bool success             = queue.try_push(elem);
    if (success) {
      st.add(retx);
    }
    return success;
  }

  /// \brief Removes the first element from the front together with any following invalid elements such that front is
  /// either a valid element or the queue is empty.
  void pop()
  {
    if (not queue.empty()) {
      st.subtract(queue.top().retx);
      queue.pop();
      clean_invalid_front();
    }
  }

  /// \brief Access the first valid element of the queue unless the queue is empty.
  /// \return First valid element in the queue
  const rlc_tx_amd_retx& front() const
  {
    srsran_assert(not queue.empty(), "Cannot return front element of empty queue.");
    srsran_assert(not queue.top().invalid, "Cannot return front element. The front is invalid.");
    return queue.top().retx;
  }

  /// \brief Replaces the first valid element of the queue.
  /// \param retx The ReTx to be written.
  void replace_front(const rlc_tx_amd_retx& retx)
  {
    srsran_assert(not queue.empty(), "Cannot replace front element of empty queue.");
    srsran_assert(not queue.top().invalid, "Cannot replace front element. The front is invalid.");
    st.subtract(queue.top().retx);
    st.add(retx);
    rlc_retx_queue_item elem = {};
    elem.retx                = retx;
    queue.top()              = elem;
  }

  /// \brief Clear the container and reset state to the initial state.
  void clear()
  {
    st = {};
    queue.clear();
  }

  /// \brief Total number of elements (valid + invalid) in the queue.
  /// \return Number of any elements in the queue.
  size_t size() const { return queue.size(); }

  /// \brief Checks the presence of at least one valid element.
  /// \return True if no valid element is queued; False if at least one valid element is queued.
  bool empty() const { return queue.empty(); }

  /// \brief Access to the internal state of the queue.
  /// \return The internal state of the queue.
  rlc_retx_queue_state state() const { return st; }

  /// Checks if the queue contains an element with a given SN.
  /// \param sn The sequence number to look up.
  /// \return true if queue contains the SN, false otherwise.
  bool has_sn(uint32_t sn) const
  {
    if (queue.empty()) {
      return false;
    }
    for (auto elem : queue) {
      if (elem.invalid == false && elem.retx.sn == sn) {
        return true;
      }
    }
    return false;
  };

  /// Checks if the queue contains an element with a given SN and also includes a specified segment interval given by a
  /// segment offset and segment length.
  /// \param sn The sequence number to look up.
  /// \param so The segment offset to check.
  /// \param length The segment length to check.
  /// \return true if queue contains the SN and the element includes the given segment interval, false otherwise.
  bool has_sn(uint32_t sn, uint32_t so, uint32_t length) const
  {
    if (queue.empty()) {
      return false;
    }
    for (auto elem : queue) {
      if (elem.invalid == false && elem.retx.sn == sn) {
        if (elem.retx.includes(so, length)) {
          return true;
        }
      }
    }
    return false;
  };

  /// \brief Removes all elements with a given SN from queue and removes any invalid elements at the front.
  ///
  /// The ReTx for matching SN will be marked as invalid and its content will be subtracted from the state.
  /// Subsequently, any invalid elements are removed from the front.
  ///
  /// \param sn sequence number to be removed from queue.
  /// \return true if at least one element was removed, false if no element to remove was found.
  bool remove_sn(uint32_t sn)
  {
    if (queue.empty()) {
      return false;
    }
    auto iter   = queue.begin();
    bool result = false;
    while (iter != queue.end()) {
      if (iter->invalid == false && iter->retx.sn == sn) {
        st.subtract(iter->retx);
        iter->invalid = true;
        result        = true;
      } else {
        ++iter;
      }
    }
    clean_invalid_front();
    return result;
  }

  /// \brief Advances the front of the queue to the first valid element or to the end of the queue.
  void clean_invalid_front()
  {
    while (!queue.empty()) {
      if (queue.top().invalid == true) {
        // remove invalid element
        queue.pop();
      } else {
        // stop at valid element
        return;
      }
    }
  }
};

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::rlc_tx_amd_retx> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_tx_amd_retx retx, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "sn={} so={} len={}", retx.sn, retx.so, retx.length);
  }
};
} // namespace fmt
