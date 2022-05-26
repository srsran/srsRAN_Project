/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/rx_softbuffer_pool.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/support/srsgnb_test.h"

namespace srsgnb {
struct rx_softbuffer_pool_description {
  unsigned max_codeblock_size;
  unsigned max_softbuffers;
  unsigned max_nof_codeblocks;
  unsigned expire_timeout_slots;
};

std::unique_ptr<rx_softbuffer_pool> create_rx_softbuffer_pool(const rx_softbuffer_pool_description& config);

} // namespace srsgnb

using namespace srsgnb;

// Tests that the pool returns nullptr when the limit of softbuffers is reached.
static void test_softbuffer_limit()
{
  // Create pool configuration for the test.
  rx_softbuffer_pool_description pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 4;
  pool_config.max_nof_codeblocks   = 4;
  pool_config.expire_timeout_slots = 10;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Create as many softbuffers as the limit is set.
  std::vector<rx_softbuffer*> softbuffers;
  for (unsigned rnti = 0; rnti != pool_config.max_softbuffers; ++rnti) {
    rx_softbuffer_identifier softbuffer_id;
    softbuffer_id.harq_ack_id = 0;
    softbuffer_id.rnti        = static_cast<uint16_t>(rnti);

    // Reserve softbuffer, it shall not fail.
    softbuffers.emplace_back(pool->reserve_softbuffer(slot, softbuffer_id, 1));
    TESTASSERT(softbuffers.back());
  }

  // Create one more softbuffer. No softbuffers are available. It must fail to reserve.
  rx_softbuffer_identifier softbuffer_id;
  softbuffer_id.harq_ack_id = 0;
  softbuffer_id.rnti        = static_cast<uint16_t>(pool_config.max_softbuffers);
  softbuffers.emplace_back(pool->reserve_softbuffer(slot, softbuffer_id, 1));
  TESTASSERT(softbuffers.back() == nullptr);
}

// Tests that the pool returns nullptr when the limit of codeblocks is reached.
static void test_codeblock_limit()
{
  // Create pool configuration for the test.
  rx_softbuffer_pool_description pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 2;
  pool_config.max_nof_codeblocks   = 1;
  pool_config.expire_timeout_slots = 10;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  rx_softbuffer_identifier softbuffer_id0;
  softbuffer_id0.harq_ack_id = 0x3;
  softbuffer_id0.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks) != nullptr);

  // Create one more softbuffer. No codeblocks are available. It must fail to reserve.
  rx_softbuffer_identifier softbuffer_id1;
  softbuffer_id1.harq_ack_id = softbuffer_id0.harq_ack_id + 1;
  softbuffer_id1.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id1, 1) == nullptr);
}

// Tests that the pool frees reserved softbuffer.
static void test_softbuffer_free()
{
  // Create pool configuration for the test.
  rx_softbuffer_pool_description pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 1;
  pool_config.max_nof_codeblocks   = 1;
  pool_config.expire_timeout_slots = 10;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  rx_softbuffer_identifier softbuffer_id0;
  softbuffer_id0.harq_ack_id = 0x3;
  softbuffer_id0.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks) != nullptr);

  // Reserve softbuffer with the same identifier. It shall not fail.
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks) != nullptr);

  // Reserve softbuffer with a different identifier. It shall fail.
  rx_softbuffer_identifier softbuffer_id1;
  softbuffer_id1.harq_ack_id = softbuffer_id0.harq_ack_id + 1;
  softbuffer_id1.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id1, pool_config.max_nof_codeblocks) == nullptr);

  // Free the first softbuffer identifier.
  pool->free_softbuffer(softbuffer_id0);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id1, pool_config.max_nof_codeblocks) != nullptr);
}

// Tests that the pool expires softbuffers after the last reserved slot.
static void test_softbuffer_expire()
{
  unsigned delay = 3;

  // Create pool configuration for the test.
  rx_softbuffer_pool_description pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.max_softbuffers      = 1;
  pool_config.max_nof_codeblocks   = 1;
  pool_config.expire_timeout_slots = 4;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Reserve softbuffer with all the codeblocks, it shall not fail.
  rx_softbuffer_identifier softbuffer_id0;
  softbuffer_id0.harq_ack_id = 0x3;
  softbuffer_id0.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks) != nullptr);

  // Run slot and reserve the same softbuffer.
  slot += delay;
  pool->run_slot(slot);
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id0, pool_config.max_nof_codeblocks) != nullptr);

  // Run for each slot until it expires.
  do {
    // Try to reserve another buffer. As there are no buffers available it shall fail.
    rx_softbuffer_identifier softbuffer_id1;
    softbuffer_id1.harq_ack_id = softbuffer_id0.harq_ack_id + 1;
    softbuffer_id1.rnti        = 0x1234;
    TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id1, pool_config.max_nof_codeblocks) == nullptr);
    ++slot;
    pool->run_slot(slot);
  } while (slot.system_slot() < pool_config.expire_timeout_slots + delay);

  // After the first buffer expired, buffer reservation shall not fail.
  rx_softbuffer_identifier softbuffer_id2;
  softbuffer_id2.harq_ack_id = softbuffer_id0.harq_ack_id + 2;
  softbuffer_id2.rnti        = 0x1234;
  TESTASSERT(pool->reserve_softbuffer(slot, softbuffer_id2, pool_config.max_nof_codeblocks) != nullptr);
}

// Tests softbuffer soft bits contents persists between retransmissions.
static void test_softbuffer_contents()
{
  unsigned nof_cb_x_buffer = 2;
  unsigned cb_size         = 16;
  // Data size cannot be larger than cb_size / 3 (recall that 1/3 is the maximum coding rate).
  unsigned data_size = 5;

  // Create pool configuration for the test.
  rx_softbuffer_pool_description pool_config;
  pool_config.max_codeblock_size   = cb_size;
  pool_config.max_softbuffers      = 4;
  pool_config.max_nof_codeblocks   = pool_config.max_softbuffers * nof_cb_x_buffer;
  pool_config.expire_timeout_slots = 10;

  // Current slot.
  slot_point slot(0, 0);

  // Create Rx softbuffer pool.
  std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
  TESTASSERT(pool);

  // Create as many softbuffers as the limit is set.
  rx_softbuffer_identifier softbuffer_id;
  softbuffer_id.harq_ack_id = 0x3;
  softbuffer_id.rnti        = 0x1234;

  // Reserve softbuffer, it shall not fail.
  rx_softbuffer* softbuffer0 = pool->reserve_softbuffer(slot, softbuffer_id, nof_cb_x_buffer);
  TESTASSERT(softbuffer0 != nullptr);

  // For each codeblock...
  for (unsigned cb_id = 0; cb_id != nof_cb_x_buffer; ++cb_id) {
    // Get codeblock soft and data bits.
    span<int8_t>  buffer      = softbuffer0->get_codeblock_soft_bits(cb_id, cb_size);
    span<uint8_t> data_buffer = softbuffer0->get_codeblock_data_bits(cb_id, data_size);

    // Make sure size matches.
    TESTASSERT(buffer.size() == cb_size);
    TESTASSERT(data_buffer.size() == data_size);

    // Write data in codeblock.
    for (unsigned bit_idx = 0; bit_idx != cb_size; ++bit_idx) {
      int8_t data     = (cb_id << 4) | bit_idx;
      buffer[bit_idx] = data;
      if (bit_idx < data_size) {
        data_buffer[bit_idx] = bit_idx & 1U;
      }
    }
  }

  // Reserve softbuffer, it shall not fail.
  rx_softbuffer* softbuffer1 = pool->reserve_softbuffer(slot, softbuffer_id, nof_cb_x_buffer);
  TESTASSERT(softbuffer1 != nullptr);

  // For each codeblock...
  for (unsigned cb_id = 0; cb_id != nof_cb_x_buffer; ++cb_id) {
    // Get codeblock soft bits.
    span<int8_t>  buffer0      = softbuffer0->get_codeblock_soft_bits(cb_id, cb_size);
    span<int8_t>  buffer1      = softbuffer1->get_codeblock_soft_bits(cb_id, cb_size);
    span<uint8_t> data_buffer0 = softbuffer0->get_codeblock_data_bits(cb_id, data_size);
    span<uint8_t> data_buffer1 = softbuffer1->get_codeblock_data_bits(cb_id, data_size);

    // Make sure the data pointers match.
    TESTASSERT(buffer0.data() == buffer1.data());
    TESTASSERT(data_buffer0.data() == data_buffer1.data());

    // Validate data persists in the codeblock.
    for (unsigned bit_idx = 0; bit_idx != cb_size; ++bit_idx) {
      int8_t data = (cb_id << 4) | bit_idx;
      TESTASSERT_EQ(buffer0[bit_idx], data);
      if (bit_idx < data_size) {
        TESTASSERT_EQ(data_buffer0[bit_idx], bit_idx & 1U);
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