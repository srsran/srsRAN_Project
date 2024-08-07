/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace srsran {

template <class T>
class sdu_window
{
public:
  virtual ~sdu_window()                    = default;
  virtual T&     add_sn(size_t sn)         = 0;
  virtual void   remove_sn(size_t sn)      = 0;
  virtual T&     operator[](size_t sn)     = 0;
  virtual size_t size() const              = 0;
  virtual bool   empty() const             = 0;
  virtual bool   full() const              = 0;
  virtual void   clear()                   = 0;
  virtual bool   has_sn(uint32_t sn) const = 0;
};

} // namespace srsran
