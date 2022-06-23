
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
  byte_buffer              buf       = {0x00, 0x01};
  std::unique_ptr<rlc_sdu> write_sdu = std::make_unique<rlc_sdu>(10, std::move(buf));
  TESTASSERT(tx_queue.write(std::move(write_sdu)));

  // Check basic stats
  TESTASSERT_EQ(1, tx_queue.size_sdus());
  TESTASSERT_EQ(2, tx_queue.size_bytes());

  // Read one SDU
  std::unique_ptr<rlc_sdu> read_sdu = nullptr;
  TESTASSERT(tx_queue.read(read_sdu));

  // Check basic stats
  TESTASSERT_EQ(0, tx_queue.size_sdus());
  TESTASSERT_EQ(0, tx_queue.size_bytes());

  // Check SDU
  byte_buffer expected_msg({0x00, 0x01});
  TESTASSERT_EQ(10, read_sdu->pdcp_sn);
  TESTASSERT(expected_msg == read_sdu->buf);
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
    std::unique_ptr<rlc_sdu> write_sdu = std::make_unique<rlc_sdu>(pdcp_sn, std::move(buf));
    if (pdcp_sn != capacity) {
      TESTASSERT(tx_queue.write(std::move(write_sdu)) == true);
    } else {
      TESTASSERT(tx_queue.write(std::move(write_sdu)) == false);
    }
  }
  TESTASSERT_EQ(capacity, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * capacity, tx_queue.size_bytes());

  // Read all SDUs and try to read on SDU over capacity
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity + 1; pdcp_sn++) {
    byte_buffer expected_msg = {};
    expected_msg.append(pdcp_sn);
    expected_msg.append(pdcp_sn);
    std::unique_ptr<rlc_sdu> read_sdu = nullptr;
    if (pdcp_sn != capacity) {
      TESTASSERT(tx_queue.read(read_sdu));
      TESTASSERT(expected_msg == read_sdu->buf);
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
  unsigned            capacity = 5;
  rlc_sdu_queue       tx_queue(capacity);

  // Fill SDU queue with SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity; pdcp_sn++) {
    byte_buffer buf = {};
    buf.append(pdcp_sn);
    buf.append(pdcp_sn);
    std::unique_ptr<rlc_sdu> write_sdu = std::make_unique<rlc_sdu>(pdcp_sn, std::move(buf));
    TESTASSERT(tx_queue.write(std::move(write_sdu)) == true);
  }
  TESTASSERT_EQ(capacity, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * capacity, tx_queue.size_bytes());

  // Discard PDCP SN 2 and 4
  TESTASSERT(tx_queue.discard_sn(2));
  TESTASSERT(tx_queue.discard_sn(4));

  // Try to discard non-existing SN
  TESTASSERT(false == tx_queue.discard_sn(16));

  // Double check correct number of SDUs and SDU bytes
  unsigned leftover_sdus = 3;
  TESTASSERT_EQ(leftover_sdus, tx_queue.size_sdus());
  TESTASSERT_EQ(leftover_sdus * 2, tx_queue.size_bytes());
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
