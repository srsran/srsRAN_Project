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

#include "srsgnb/phy/upper/rx_softbuffer.h"

namespace srsgnb {

class rx_softbuffer_spy : public rx_softbuffer
{
public:
  unsigned int get_nof_codeblocks() const override
  {
    ++const_count;
    return 0;
  }
  void       reset_codeblocks_crc() override { ++count; }
  span<bool> get_codeblocks_crc() override
  {
    ++count;
    return {};
  }
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned /**/, unsigned /**/) override
  {
    ++count;
    return {};
  }
  span<uint8_t> get_codeblock_data_bits(unsigned /**/, unsigned /**/) override
  {
    ++count;
    return {};
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
};

} // namespace srsgnb