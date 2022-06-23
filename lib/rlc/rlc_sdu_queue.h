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
#include "srsgnb/rlc/rlc.h"
#include <cstdint>
#include <list>

namespace srsgnb {

class rlc_sdu_queue
{
public:
  explicit rlc_sdu_queue(uint16_t capacity = 256) : capacity(capacity) {}

  bool write(rlc_sdu sdu)
  {
    if (queue.size() >= capacity) {
      return false;
    }
    unread_bytes += sdu.buf.length();
    queue.push_back(std::move(sdu));
    return true;
  }

  bool read(rlc_sdu& sdu)
  {
    if (is_empty()) {
      return false;
    }
    sdu = std::move(queue.front());
    unread_bytes -= sdu.buf.length();
    queue.pop_front();
    return true;
  }

  uint32_t size_sdus() const { return queue.size(); }

  uint32_t size_bytes() const { return unread_bytes; }

  bool is_empty() { return queue.empty(); }

  bool is_full() { return queue.size() >= capacity; }

  bool discard_sn(uint32_t pdcp_sn)
  {
    for (std::list<rlc_sdu>::iterator it = queue.begin(); it != queue.end(); ++it) {
      if (it->pdcp_sn == pdcp_sn) {
        unread_bytes -= it->buf.length();
        queue.erase(it);
        return true;
      }
    }
    return false;
  }

private:
  uint32_t           unread_bytes = 0;
  uint16_t           capacity     = 0;
  std::list<rlc_sdu> queue;
};

} // namespace srsgnb
#endif
