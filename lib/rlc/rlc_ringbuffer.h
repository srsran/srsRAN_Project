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

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <cstdint>

namespace srsgnb {

template <class T>
struct rlc_ringbuffer_base {
  virtual ~rlc_ringbuffer_base()           = default;
  virtual T&     add_pdu(size_t sn)        = 0;
  virtual void   remove_pdu(size_t sn)     = 0;
  virtual T&     operator[](size_t sn)     = 0;
  virtual size_t size() const              = 0;
  virtual bool   empty() const             = 0;
  virtual bool   full() const              = 0;
  virtual void   clear()                   = 0;
  virtual bool   has_sn(uint32_t sn) const = 0;
};

/// \brief This class provides a container for the Tx/Rx windows holding RLC PDUs, indexed by Sequence Numbers (SN)
/// @tparam T storage type
/// @tparam WINDOW_SIZE size of the RLC AM window
template <class T, std::size_t WINDOW_SIZE>
struct rlc_ringbuffer : public rlc_ringbuffer_base<T> {
  ~rlc_ringbuffer() = default;

  T& add_pdu(size_t sn) override
  {
    srsgnb_assert(not has_sn(sn), "The same SN={} should not be added twice", sn);
    window.overwrite(sn, T(sn));
    return window[sn];
  }
  void remove_pdu(size_t sn) override
  {
    srsgnb_assert(has_sn(sn), "The removed SN={} is not in the window", sn);
    window.erase(sn);
  }
  T&     operator[](size_t sn) override { return window[sn]; }
  size_t size() const override { return window.size(); }
  bool   full() const override { return window.full(); }
  bool   empty() const override { return window.empty(); }
  void   clear() override { window.clear(); }

  bool has_sn(uint32_t sn) const override { return window.contains(sn); }

private:
  srsgnb::circular_map<uint32_t, T, WINDOW_SIZE> window;
};

} // namespace srsgnb
