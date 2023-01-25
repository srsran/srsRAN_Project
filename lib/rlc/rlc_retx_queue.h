/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_am_pdu.h"
#include "srsgnb/support/srsgnb_assert.h"
#include "fmt/format.h"
#include <cstdint>
#include <list>

namespace srsgnb {

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

class rlc_retx_queue
{
  std::list<rlc_tx_amd_retx> queue;
  rlc_retx_queue_state       st;

public:
  ~rlc_retx_queue() = default;

  void push(const rlc_tx_amd_retx& elem)
  {
    st.add(elem);
    queue.push_back(elem);
  }

  void pop()
  {
    if (not queue.empty()) {
      st.subtract(queue.front());
      queue.pop_front();
    }
  }

  const rlc_tx_amd_retx& front() const
  {
    srsgnb_assert(not queue.empty(), "Cannot return front element of empty queue");
    return queue.front();
  }

  void replace_front(const rlc_tx_amd_retx& elem)
  {
    srsgnb_assert(not queue.empty(), "Cannot replace front of empty queue");
    st.subtract(queue.front());
    st.add(elem);
    queue.front() = elem;
  }

  const std::list<rlc_tx_amd_retx>& get_inner_queue() const { return queue; }

  void clear()
  {
    st = {};
    queue.clear();
  }
  size_t size() const { return queue.size(); }
  bool   empty() const { return queue.empty(); }

  rlc_retx_queue_state state() const { return st; }

  /// Checks if the queue contains an element with a given SN.
  /// \param sn The sequence number to look up
  /// \return true if queue contains the SN, false otherwise
  bool has_sn(uint32_t sn) const
  {
    if (queue.empty()) {
      return false;
    }
    for (auto elem : queue) {
      if (elem.sn == sn) {
        return true;
      }
    }
    return false;
  };

  /// Checks if the queue contains an element with a given SN and also includes a specified segment interval given by a
  /// segment offset and segment length.
  /// \param sn The sequence number to look up
  /// \param so The segment offset to check
  /// \param length The segment length to check
  /// \return true if queue contains the SN and the element includes the given segment interval, false otherwise
  bool has_sn(uint32_t sn, uint32_t so, uint32_t length) const
  {
    if (queue.empty()) {
      return false;
    }
    for (auto elem : queue) {
      if (elem.sn == sn) {
        if (elem.includes(so, length)) {
          return true;
        }
      }
    }
    return false;
  };

  /// \brief Removes SN from queue and returns after first match
  /// \param sn sequence number to be removed from queue
  /// \return true if one element was removed, false if no element to remove was found
  bool remove_sn(uint32_t sn)
  {
    if (queue.empty()) {
      return false;
    }
    auto iter = queue.begin();
    while (iter != queue.end()) {
      if (iter->sn == sn) {
        st.subtract(*iter);
        iter = queue.erase(iter);
        return true;
      } else {
        ++iter;
      }
    }
    return false;
  }
};

} // namespace srsgnb

namespace fmt {
template <>
struct formatter<srsgnb::rlc_tx_amd_retx> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::rlc_tx_amd_retx retx, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "SN={}, SO={}, length={}", retx.sn, retx.so, retx.length);
  }
};
} // namespace fmt
