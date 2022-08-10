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
#include <cstdint>
#include <list>

namespace srsgnb {

struct rlc_tx_amd_retx {
  uint32_t sn             = INVALID_RLC_SN; ///< sequence number
  bool     is_segment     = false;          ///< flag whether this is a segment or not
  uint32_t so_start       = 0;              ///< offset to first byte of this segment
  uint32_t segment_length = 0;              ///< number of bytes contained in this segment
  uint32_t current_so     = 0;              ///< stores progressing SO during segmentation of this object

  /// \brief Implements a check whether the range of this retransmission object includes the given segment offset
  /// \param segment_offset The segment offset to check
  /// \return true if the segment offset is covered by the retransmission object. Otherwise false
  bool includes(uint32_t segment_offset) const
  {
    return (segment_offset >= so_start) && (segment_offset < current_so + segment_length);
  }
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
        if (elem.overlaps(so)) {
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
