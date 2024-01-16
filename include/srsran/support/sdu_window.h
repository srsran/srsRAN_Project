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

#include <cstdint>
#include <stddef.h>

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
