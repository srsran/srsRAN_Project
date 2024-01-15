
/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/rlc/rlc_sdu_queue.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/test_utils.h"

namespace srsran {

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
  TESTASSERT(read_sdu.pdcp_sn.has_value());
  TESTASSERT_EQ(10, read_sdu.pdcp_sn.value());
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
    TESTASSERT(buf.append(pdcp_sn));
    TESTASSERT(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
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
    TESTASSERT(expected_msg.append(pdcp_sn));
    TESTASSERT(expected_msg.append(pdcp_sn));
    rlc_sdu read_sdu = {};
    if (pdcp_sn != capacity) {
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
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    byte_buffer buf = {};
    TESTASSERT(buf.append(pdcp_sn));
    TESTASSERT(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    TESTASSERT(tx_queue.write(std::move(write_sdu)) == true);
  }
  TESTASSERT_EQ(n_sdus, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * n_sdus, tx_queue.size_bytes());

  // Discard pdcp_sn 2 and 4
  TESTASSERT(tx_queue.discard(2));
  TESTASSERT(tx_queue.discard(4));

  // Try to discard non-existing pdcp_sn
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
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    byte_buffer buf = {};
    TESTASSERT(buf.append(pdcp_sn));
    TESTASSERT(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    TESTASSERT(tx_queue.write(std::move(write_sdu)) == true);
  }
  TESTASSERT_EQ(n_sdus, tx_queue.size_sdus());
  TESTASSERT_EQ(2 * n_sdus, tx_queue.size_bytes());

  // Discard all SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    TESTASSERT(tx_queue.discard(pdcp_sn));
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
} // namespace srsran

int main()
{
  srslog::init();
  srslog::fetch_basic_logger("TEST", false).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
  fprintf(stdout, "Testing RLC SDU queue\n");

  srsran::queue_unqueue_test();
  srsran::full_capacity_test();
  srsran::discard_test();
  srsran::discard_all_test();
}
