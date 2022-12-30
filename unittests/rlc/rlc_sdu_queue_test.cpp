
/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/rlc/rlc_sdu_queue.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

void queue_unqueue_test()
{
  test_delimit_logger delimiter{"RLC SDU queue unqueue test"};
  rlc_sdu_queue       tx_queue;

  // Write 1 SDU
  byte_buffer buf       = {0x00, 0x01};
  rlc_sdu     write_sdu = {std::move(buf), 10};
  TESTASSERT(tx_queue.write(std::move(write_sdu)));

  // Check basic stats
  TESTASSERT_EQ(1, tx_queue.size_sdus());
  TESTASSERT_EQ(2, tx_queue.size_bytes());

  // Read one SDU
  rlc_sdu read_sdu;
  TESTASSERT(tx_queue.read(read_sdu));

  // Check basic stats
  TESTASSERT_EQ(0, tx_queue.size_sdus());
  TESTASSERT_EQ(0, tx_queue.size_bytes());

  // Check SDU
  byte_buffer expected_msg({0x00, 0x01});
  TESTASSERT(read_sdu.pdcp_count.has_value());
  TESTASSERT_EQ(10, read_sdu.pdcp_count.value());
  TESTASSERT(expected_msg == read_sdu.buf);
}

void full_capacity_test()
{
  test_delimit_logger delimiter{"RLC SDU capacity test"};
  unsigned            capacity = 5;
  rlc_sdu_queue       tx_queue(capacity);

  // Write Capacity + 1 SDUs
  for (uint32_t pdcp_count = 0; pdcp_count < capacity + 1; pdcp_count++) {
    byte_buffer buf = {};
    buf.append(pdcp_count);
    buf.append(pdcp_count);
    rlc_sdu write_sdu = {std::move(buf), pdcp_count};
    if (pdcp_count != capacity) {
      TESTASSERT(tx_queue.write(std::move(write_sdu)) == true);
    } else {
      TESTASSERT(tx_queue.write(std::move(write_sdu)) == false);
    }
  }
  TESTASSERT_EQ(capacity, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * capacity, tx_queue.size_bytes());

  // Read all SDUs and try to read on SDU over capacity
  for (uint32_t pdcp_count = 0; pdcp_count < capacity + 1; pdcp_count++) {
    byte_buffer expected_msg = {};
    expected_msg.append(pdcp_count);
    expected_msg.append(pdcp_count);
    rlc_sdu read_sdu = {};
    if (pdcp_count != capacity) {
      TESTASSERT(tx_queue.read(read_sdu));
      TESTASSERT(expected_msg == read_sdu.buf);
    } else {
      TESTASSERT(false == tx_queue.read(read_sdu));
    }
  }

  TESTASSERT_EQ(0, tx_queue.size_sdus());
  TESTASSERT_EQ(0, tx_queue.size_bytes());
}

void discard_test()
{
  test_delimit_logger delimiter{"RLC SDU discard test"};
  unsigned            capacity = 10;
  unsigned            n_sdus   = capacity;
  rlc_sdu_queue       tx_queue(capacity);

  // Fill SDU queue with SDUs
  for (uint32_t pdcp_count = 0; pdcp_count < n_sdus; pdcp_count++) {
    byte_buffer buf = {};
    buf.append(pdcp_count);
    buf.append(pdcp_count);
    rlc_sdu write_sdu = {std::move(buf), pdcp_count};
    TESTASSERT(tx_queue.write(std::move(write_sdu)) == true);
  }
  TESTASSERT_EQ(n_sdus, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * n_sdus, tx_queue.size_bytes());

  // Discard pdcp_count 2 and 4
  TESTASSERT(tx_queue.discard(2));
  TESTASSERT(tx_queue.discard(4));

  // Try to discard non-existing pdcp_count
  TESTASSERT(false == tx_queue.discard(16));

  // Double check correct number of SDUs and SDU bytes
  unsigned leftover_sdus = n_sdus - 2;
  TESTASSERT_EQ(leftover_sdus, tx_queue.size_sdus());
  TESTASSERT_EQ(leftover_sdus * 2, tx_queue.size_bytes());

  // Read SDUs
  for (uint32_t n = 0; n < leftover_sdus; n++) {
    rlc_sdu read_sdu = {};
    TESTASSERT(tx_queue.read(read_sdu));
  }
  TESTASSERT_EQ(0, tx_queue.size_sdus());
  TESTASSERT_EQ(0, tx_queue.size_bytes());
}

void discard_all_test()
{
  test_delimit_logger delimiter{"RLC SDU discard all test"};
  unsigned            capacity = 10;
  unsigned            n_sdus   = capacity / 2;
  rlc_sdu_queue       tx_queue(capacity);

  // Fill SDU queue with SDUs
  for (uint32_t pdcp_count = 0; pdcp_count < n_sdus; pdcp_count++) {
    byte_buffer buf = {};
    buf.append(pdcp_count);
    buf.append(pdcp_count);
    rlc_sdu write_sdu = {std::move(buf), pdcp_count};
    TESTASSERT(tx_queue.write(std::move(write_sdu)) == true);
  }
  TESTASSERT_EQ(n_sdus, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * n_sdus, tx_queue.size_bytes());

  // Discard all SDUs
  for (uint32_t pdcp_count = 0; pdcp_count < n_sdus; pdcp_count++) {
    TESTASSERT(tx_queue.discard(pdcp_count));
  }

  TESTASSERT_EQ(0, tx_queue.size_sdus());
  TESTASSERT_EQ(0, tx_queue.size_bytes());

  // Read SDU
  {
    rlc_sdu read_sdu = {};
    TESTASSERT(tx_queue.read(read_sdu) == false);
  }
  TESTASSERT_EQ(0, tx_queue.size_sdus());
  TESTASSERT_EQ(0, tx_queue.size_bytes());
}
} // namespace srsgnb

int main()
{
  srslog::init();
  srslog::fetch_basic_logger("TEST", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
  fprintf(stdout, "Testing RLC SDU queue\n");

  srsgnb::queue_unqueue_test();
  srsgnb::full_capacity_test();
  srsgnb::discard_test();
  srsgnb::discard_all_test();
}
