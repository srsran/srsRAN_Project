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
  const static uint32_t invalid_rlc_sn = std::numeric_limits<uint32_t>::max();

  uint32_t sn             = INVALID_RLC_SN; ///< sequence number
  bool     is_segment     = false;          ///< flag whether this is a segment or not
  uint32_t so_start       = 0;              ///< offset to first byte of this segment
  uint32_t segment_length = 0;              ///< number of bytes contained in this segment
  uint32_t current_so     = 0;              ///< stores progressing SO during segmentation of this object

  /**
   * @brief overlaps implements a check whether the range of this retransmission object includes
   * the given segment offset
   * @param so the segment offset to check
   * @return true if the segment offset is covered by the retransmission object. Otherwise false
   */
  bool overlaps(uint32_t segment_offset) const
  {
    return (segment_offset >= so_start) && (segment_offset < current_so + segment_length);
  }
};

template <class T>
class pdu_retx_queue_base
{
public:
  virtual ~pdu_retx_queue_base() = default;
  virtual T&     push()          = 0;
  virtual void   pop()           = 0;
  virtual T&     front()         = 0;
  virtual void   clear()         = 0;
  virtual size_t size() const    = 0;
  virtual bool   empty() const   = 0;
  virtual bool   full() const    = 0;

  virtual T&       operator[](size_t idx)       = 0;
  virtual const T& operator[](size_t idx) const = 0;

  virtual bool has_sn(uint32_t sn) const              = 0;
  virtual bool has_sn(uint32_t sn, uint32_t so) const = 0;
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

  /**
   * @brief remove_sn removes SN from queue and returns after first match
   * @param sn sequence number to be removed from queue
   * @return true if one element was removed, false if no element to remove was found
   */
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
