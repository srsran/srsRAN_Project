/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/tensor.h"
#include "srsran/phy/upper/unique_tx_buffer.h"

namespace srsran {

/// Implements a lockable transmit buffer spy.
class tx_buffer_spy : public unique_tx_buffer::lockable_buffer
{
public:
  /// Default constructor - creates a buffer without any storage.
  tx_buffer_spy() = default;

  /// Constructs a transmit buffer that can store a maximum number of codeblocks of a certain size.
  tx_buffer_spy(unsigned max_codeblock_size, unsigned nof_codeblocks) : data({max_codeblock_size, nof_codeblocks}) {}

  // See interface for documentation.
  unsigned get_nof_codeblocks() const override
  {
    ++const_count;
    return data.size();
  }

  // See interface for documentation.
  bit_buffer get_codeblock(unsigned codeblock_id, unsigned data_size) override
  {
    ++count;
    data[codeblock_id].resize(data_size);
    return data[codeblock_id];
  }

  // See interface for documentation.
  unsigned get_absolute_codeblock_id(unsigned codeblock_id) const override
  {
    ++const_count;
    return codeblock_id;
  }

  // See interface for documentation.
  void lock() override { locked = true; }

  // See interface for documentation.
  void unlock() override { locked = false; }

  /// Clears all counters.
  void clear()
  {
    count       = 0;
    const_count = 0;
  }

  /// Get the total number of times the methods have been called.
  unsigned get_total_count() const { return count + const_count; }

  /// Returns true if the buffer is locked.
  bool is_locked() const { return locked; }

private:
  unsigned                        count       = 0;
  mutable unsigned                const_count = 0;
  bool                            locked      = false;
  std::vector<dynamic_bit_buffer> data;
};

} // namespace srsran