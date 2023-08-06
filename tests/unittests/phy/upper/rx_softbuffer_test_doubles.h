/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/tensor.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"

namespace srsran {

class rx_softbuffer_spy : public unique_rx_softbuffer::softbuffer
{
public:
  rx_softbuffer_spy() = default;

  rx_softbuffer_spy(unsigned max_codeblock_size, unsigned nof_codeblocks) :
    soft_bits({max_codeblock_size, nof_codeblocks}), crc(nof_codeblocks)
  {
    for (unsigned i_cb = 0; i_cb != nof_codeblocks; ++i_cb) {
      hard_bits.emplace_back(dynamic_bit_buffer(max_codeblock_size));
    }
  }

  unsigned int get_nof_codeblocks() const override
  {
    ++const_count;
    return soft_bits.get_dimensions_size().back();
  }
  void       reset_codeblocks_crc() override { ++count; }
  span<bool> get_codeblocks_crc() override
  {
    ++count;
    return span<bool>(reinterpret_cast<bool*>(crc.data()), crc.size());
  }
  unsigned get_absolute_codeblock_id(unsigned codeblock_id) const override
  {
    ++const_count;
    return 0;
  }
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned data_size) override
  {
    ++count;
    return soft_bits.get_view<1>({codeblock_id}).first(data_size);
  }
  bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    ++count;
    return hard_bits[codeblock_id].first(data_size);
  }

  void lock() override { locked = true; }

  void unlock() override { locked = false; }

  void release() override { locked = false; }

  /// Clears all counters.
  void clear()
  {
    count       = 0;
    const_count = 0;
  }

  /// Get the total number of times the methods have been called.
  unsigned get_total_count() const { return count + const_count; }

  /// Returns true if the softbuffer is locked.
  bool is_locked() const { return locked; }

private:
  unsigned         count       = 0;
  mutable unsigned const_count = 0;
  bool             locked      = false;

  dynamic_tensor<2, log_likelihood_ratio> soft_bits;
  std::vector<dynamic_bit_buffer>         hard_bits;
  std::vector<uint8_t>                    crc;
};

} // namespace srsran