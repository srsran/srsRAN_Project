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

#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/phy/upper/rx_softbuffer_pool.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/srsvec/compare.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

// Tests that the pool returns nullptr when the limit of softbuffers is reached.
static void test_softbuffer_limit()
{
  // Create pool configuration for the test.
  rx_softbuffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 4;
  pool_config.max_nof_codeblocks   = 4;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Create as many softbuffers as the limit is set.
  std::vector<unique_rx_softbuffer> softbuffers;
  for (unsigned rnti = 0; rnti != pool_config.max_softbuffers; ++rnti) {
    rx_softbuffer_identifier softbuffer_id;
    softbuffer_id.harq_ack_id = 0;
    softbuffer_id.rnti        = static_cast<uint16_t>(rnti);

    // Reserve softbuffer, it shall not fail.
    softbuffers.emplace_back(pool->reserve_softbuffer(slot, softbuffer_id, 1));
    TESTASSERT(softbuffers.back().is_valid());
  }

  // Create one more softbuffer. No softbuffers are available. It must fail to reserve.
  rx_softbuffer_identifier softbuffer_id;
  softbuffer_id.harq_ack_id = 0;
  softbuffer_id.rnti        = static_cast<uint16_t>(pool_config.max_softbuffers);
  softbuffers.emplace_back(pool->reserve_softbuffer(slot, softbuffer_id, 1));
  TESTASSERT(!softbuffers.back().is_valid());
}

// Tests that the pool returns nullptr when the limit of codeblocks is reached.
static void test_codeblock_limit()
{
  // Create pool configuration for the test.
  rx_softbuffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 2;
  pool_config.max_nof_codeblocks   = 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  rx_softbuffer_identifier softbuffer_id0;
  softbuffer_id0.harq_ack_id      = 0x3;
  softbuffer_id0.rnti             = 0x1234;
  unique_rx_softbuffer softbuffer = pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks);
  TESTASSERT(softbuffer.is_valid());

  // Create one more softbuffer. No codeblocks are available. It must fail to reserve.
  rx_softbuffer_identifier softbuffer_id1;
  softbuffer_id1.harq_ack_id = softbuffer_id0.harq_ack_id + 1;
  softbuffer_id1.rnti        = 0x1234;
  TESTASSERT(!pool->reserve_softbuffer(slot, softbuffer_id1, pool_config.max_nof_codeblocks).is_valid());
}

// Tests that the pool frees reserved softbuffer.
static void test_softbuffer_free()
{
  // Create pool configuration for the test.
  rx_softbuffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 1;
  pool_config.max_nof_codeblocks   = 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  rx_softbuffer_identifier softbuffer_id0;
  softbuffer_id0.harq_ack_id = 0x3;
  softbuffer_id0.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks).is_valid());

  // Reserve softbuffer with the same identifier. It shall not fail.
  unique_rx_softbuffer softbuffer = pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks);
  TESTASSERT(softbuffer.is_valid());

  // Reserve softbuffer with a different identifier. It shall fail.
  rx_softbuffer_identifier softbuffer_id1;
  softbuffer_id1.harq_ack_id = softbuffer_id0.harq_ack_id + 1;
  softbuffer_id1.rnti        = 0x1234;
  TESTASSERT(!pool->reserve_softbuffer(slot, softbuffer_id1, pool_config.max_nof_codeblocks).is_valid());

  // Free the first softbuffer identifier.
  softbuffer.release();

  // Run slot for clearing the buffer.
  pool->run_slot(slot);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id1, pool_config.max_nof_codeblocks).is_valid());
}

// Tests that the pool expires softbuffers after the last reserved slot.
static void test_softbuffer_expire()
{
  unsigned delay = 3;

  // Create pool configuration for the test.
  rx_softbuffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 1;
  pool_config.max_nof_codeblocks   = 1;
  pool_config.expire_timeout_slots = 4;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  rx_softbuffer_identifier softbuffer_id0;
  softbuffer_id0.harq_ack_id = 0x3;
  softbuffer_id0.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks).is_valid());

  // Run slot and reserve the same softbuffer.
  slot += delay;
  pool->run_slot(slot);
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks).is_valid());

  // Run for each slot until it expires.
  do {
    // Try to reserve another buffer. As there are no buffers available it shall fail.
    rx_softbuffer_identifier softbuffer_id1;
    softbuffer_id1.harq_ack_id = softbuffer_id0.harq_ack_id + 1;
    softbuffer_id1.rnti        = 0x1234;
    TESTASSERT(!pool->reserve_softbuffer(slot, softbuffer_id1, pool_config.max_nof_codeblocks).is_valid());
    ++slot;
    pool->run_slot(slot);
  } while (slot.system_slot() < pool_config.expire_timeout_slots + delay);

  // After the first buffer expired, buffer reservation shall not fail.
  rx_softbuffer_identifier softbuffer_id2;
  softbuffer_id2.harq_ack_id = softbuffer_id0.harq_ack_id + 2;
  softbuffer_id2.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id2, pool_config.max_nof_codeblocks).is_valid());
}

// Tests softbuffer soft bits contents persists between retransmissions.
static void test_softbuffer_contents()
{
  unsigned nof_cb_x_buffer = 2;
  unsigned cb_size         = 16;
  // Data size cannot be larger than cb_size / 3 (recall that 1/3 is the maximum coding rate).
  unsigned data_size = 5;

  // Create pool configuration for the test.
  rx_softbuffer_pool_config pool_config;
  pool_config.max_codeblock_size   = cb_size;
  pool_config.max_softbuffers      = 4;
  pool_config.max_nof_codeblocks   = pool_config.max_softbuffers * nof_cb_x_buffer;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Create as many softbuffers as the limit is set.
  rx_softbuffer_identifier softbuffer_id;
  softbuffer_id.harq_ack_id = 0x3;
  softbuffer_id.rnti        = 0x1234;

  // Temporal storage of softbuffer codeblock information.
  std::vector<span<log_likelihood_ratio>> cb_soft_bits;
  std::vector<bit_buffer>                 cb_data_bits;

  // Note: two softbuffers with the same identifier cannot be simultaneously in scope.
  {
    // Reserve softbuffer, it shall not fail.
    unique_rx_softbuffer softbuffer = pool->reserve_softbuffer(slot, softbuffer_id, nof_cb_x_buffer);
    TESTASSERT(softbuffer.is_valid());

    // For each codeblock...
    for (unsigned cb_id = 0; cb_id != nof_cb_x_buffer; ++cb_id) {
      // Get codeblock soft and data bits.
      span<log_likelihood_ratio> buffer      = softbuffer.get().get_codeblock_soft_bits(cb_id, cb_size);
      bit_buffer                 data_buffer = softbuffer.get().get_codeblock_data_bits(cb_id, data_size);

      cb_soft_bits.emplace_back(buffer);
      cb_data_bits.emplace_back(data_buffer);

      // Make sure size matches.
      TESTASSERT(buffer.size() == cb_size);
      TESTASSERT(data_buffer.size() == data_size);

      // Write data in codeblock.
      for (unsigned bit_idx = 0; bit_idx != cb_size; ++bit_idx) {
        int8_t data     = (cb_id << 4) | bit_idx;
        buffer[bit_idx] = data;
        if (bit_idx < data_size) {
          data_buffer.insert(bit_idx & 1U, bit_idx, 1);
        }
      }
    }
  }

  // Reserve softbuffer, it shall not fail.
  unique_rx_softbuffer softbuffer = pool->reserve_softbuffer(slot, softbuffer_id, nof_cb_x_buffer);
  TESTASSERT(softbuffer.is_valid());

  // For each codeblock...
  for (unsigned cb_id = 0; cb_id != nof_cb_x_buffer; ++cb_id) {
    // Get codeblock soft bits.
    span<log_likelihood_ratio> buffer0      = cb_soft_bits[cb_id];
    span<log_likelihood_ratio> buffer1      = softbuffer.get().get_codeblock_soft_bits(cb_id, cb_size);
    bit_buffer                 data_buffer0 = cb_data_bits[cb_id];
    bit_buffer                 data_buffer1 = softbuffer.get().get_codeblock_data_bits(cb_id, data_size);

    // Make sure absolute codeblock indexes match.
    TESTASSERT_EQ(softbuffer.get().get_absolute_codeblock_id(cb_id), cb_id);

    // Make sure the data pointers match.
    TESTASSERT(buffer0.data() == buffer1.data());
    TESTASSERT(data_buffer0.get_buffer().data() == data_buffer1.get_buffer().data());

    // Validate data persists in the codeblock.
    for (unsigned bit_idx = 0; bit_idx != cb_size; ++bit_idx) {
      log_likelihood_ratio data = static_cast<int>((cb_id << 4) | bit_idx);
      TESTASSERT_EQ(buffer0[bit_idx], data);
      if (bit_idx < data_size) {
        TESTASSERT_EQ(data_buffer0.extract(bit_idx, 1), bit_idx & 1U);
      }
    }
  }
}

int main()
{
  test_softbuffer_limit();
  test_codeblock_limit();
  test_softbuffer_free();
  test_softbuffer_expire();
  test_softbuffer_contents();

  return 0;
}
