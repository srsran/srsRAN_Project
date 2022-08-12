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
#include <cstdint>
#include <list>

namespace srsgnb {

struct rlc_tx_amd_retx {
  uint32_t sn     = INVALID_RLC_SN; ///< sequence number
  uint32_t so     = 0;              ///< offset to first byte of this ReTx within the associated SDU
  uint32_t length = 0;              ///< length of this ReTx

  /// \brief Implements a check whether the range of this retransmission object includes the given segment offset
  /// \param segment_offset The segment offset to check
  /// \return true if the segment offset is covered by the retransmission object. Otherwise false
  bool includes(uint32_t segment_offset) const { return (segment_offset >= so) && (segment_offset < so + length); }
};

template <class T>
class pdu_retx_queue_list
{
  std::list<T> queue;

public:
  ~pdu_retx_queue_list() = default;
  T& push()
  {
    queue.emplace_back();
    return queue.back();
  }

  void pop()
  {
    if (not queue.empty()) {
      queue.pop_front();
    }
  }

  T& front()
  {
    assert(not queue.empty());
    return queue.front();
  }

  const std::list<T>& get_inner_queue() const { return queue; }

  void   clear() { queue.clear(); }
  size_t size() const { return queue.size(); }
  bool   empty() const { return queue.empty(); }

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

  bool has_sn(uint32_t sn, uint32_t so) const
  {
    if (queue.empty()) {
      return false;
    }
    for (auto elem : queue) {
      if (elem.sn == sn) {
        if (elem.includes(so)) {
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
