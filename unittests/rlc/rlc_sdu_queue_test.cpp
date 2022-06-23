
/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/rlc/rlc_sdu_queue.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

void queue_unqueue_test()
{
  test_delimit_logger delimiter{"RLC SDU queue unqueue test"};
  rlc_sdu_queue       tx_queue;

  // Write 1 SDU
  byte_buffer buf       = {0x00, 0x01};
  rlc_sdu     write_sdu = {10, std::move(buf)};
  TESTASSERT(tx_queue.write(write_sdu));

  // Check basic stats
  TESTASSERT_EQ(1, tx_queue.size_sdus());
  TESTASSERT_EQ(2, tx_queue.size_bytes());

  // Read one SDU
  rlc_sdu read_sdu = {};
  read_sdu         = tx_queue.read();

  // Check basic stats
  TESTASSERT_EQ(0, tx_queue.size_sdus());
  TESTASSERT_EQ(0, tx_queue.size_bytes());

  // Check SDU
  byte_buffer expected_msg({0x00, 0x01});
  TESTASSERT_EQ(10, read_sdu.pdcp_sn);
  TESTASSERT(expected_msg == read_sdu.buf);
}

void full_capacity_test()
{
  test_delimit_logger delimiter{"RLC SDU capacity test"};
  unsigned            capacity = 5;
  rlc_sdu_queue       tx_queue(capacity);

  // Write Capacity + 1 SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity + 1; pdcp_sn++) {
    byte_buffer buf = {};
    buf.append(pdcp_sn);
    buf.append(pdcp_sn);
    rlc_sdu write_sdu = {pdcp_sn, std::move(buf)};
    if (pdcp_sn != capacity) {
      TESTASSERT(tx_queue.write(write_sdu) == true);
    } else {
      TESTASSERT(tx_queue.write(write_sdu) == false);
    }
  }
  TESTASSERT_EQ(capacity, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * capacity, tx_queue.size_bytes());

  // Read all SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity; pdcp_sn++) {
    byte_buffer expected_buf = {};
    expected_buf.append(pdcp_sn);
    expected_buf.append(pdcp_sn);
    rlc_sdu read_sdu = tx_queue.read();
  }
}

void discard_test()
{
  test_delimit_logger delimiter{"RLC SDU discard test"};
}

} // namespace srsgnb

int main()
{
  srslog::init();
  auto& logger = srslog::fetch_basic_logger("TEST", false);
  logger.set_level(srslog::basic_levels::debug);

  logger.info("Testing RLC SDU queue");
  srslog::flush();
  srsgnb::queue_unqueue_test();
  srsgnb::full_capacity_test();
  srsgnb::discard_test();
}
