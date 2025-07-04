/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class rx_buffer_pool_spy : public rx_buffer_pool, private unique_rx_buffer::callback
{
public:
  unique_rx_buffer
  reserve(const slot_point& slot, trx_buffer_identifier id, unsigned nof_codeblocks, bool new_data) override
  {
    if (is_locked()) {
      return unique_rx_buffer();
    }
    return unique_rx_buffer(*this);
  }
  void run_slot(const slot_point& slot) override { current_slot = slot; }

  slot_point get_current_slot() const { return current_slot; }

  bool is_locked() const { return is_buffer_locked; }

  unsigned                   get_nof_codeblocks() const override { return 0; }
  void                       reset_codeblocks_crc() override {}
  span<bool>                 get_codeblocks_crc() override { return {}; }
  unsigned                   get_absolute_codeblock_id(unsigned codeblock_id) const override { return 0; }
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    return {};
  }
  bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    static static_bit_buffer<8> buffer = {};
    return buffer;
  }

  bool try_lock() override
  {
    is_buffer_locked = true;
    return true;
  }
  void unlock() override { is_buffer_locked = false; }
  void release() override { is_buffer_locked = false; }

  void clear()
  {
    current_slot     = {};
    is_buffer_locked = false;
  }

private:
  slot_point current_slot     = {};
  bool       is_buffer_locked = false;
};

} // namespace srsran
