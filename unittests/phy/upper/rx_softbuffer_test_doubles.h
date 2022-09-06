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

#include "srsgnb/adt/tensor.h"
#include "srsgnb/phy/upper/rx_softbuffer.h"

namespace srsgnb {

class rx_softbuffer_spy : public rx_softbuffer
{
public:
  rx_softbuffer_spy() = default;

  rx_softbuffer_spy(unsigned max_codeblock_size, unsigned nof_codeblocks) :
    soft_bits({max_codeblock_size, nof_codeblocks}),
    hard_bits({max_codeblock_size, nof_codeblocks}),
    crc(nof_codeblocks)
  {
    // Do nothing.
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
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned data_size) override
  {
    ++count;
    return soft_bits.get_view<1>({codeblock_id}).first(data_size);
  }
  span<uint8_t> get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    ++count;
    return hard_bits.get_view<1>({codeblock_id}).first(data_size);
  }

  /// Clears all counters.
  void clear()
  {
    count       = 0;
    const_count = 0;
  }

  /// Get the total number of times the methods have been called.
  unsigned get_total_count() const { return count + const_count; }

private:
  unsigned         count       = 0;
  mutable unsigned const_count = 0;

  dynamic_tensor<2, log_likelihood_ratio> soft_bits;
  dynamic_tensor<2, uint8_t>              hard_bits;
  std::vector<uint8_t>                    crc;
};

} // namespace srsgnb