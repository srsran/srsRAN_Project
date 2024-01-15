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

#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/executors/task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

void write_pcap_ngap_thread_function_byte_buffer(srsran::dlt_pcap* pcap, uint32_t num_pdus);
void write_pcap_ngap_thread_function_spans(srsran::dlt_pcap* pcap, uint32_t num_pdus);

class pcap_ngap_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("PCAP").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("PCAP").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    // Start the log backend.
    srslog::init();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  task_worker           worker{"pcap", 1024};
  task_worker_executor  pcap_exec{worker};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

TEST_F(pcap_ngap_test, write_pdu)
{
  auto                    ngap_pcap_writer = create_ngap_pcap("/tmp/ngap_write_pdu.pcap", pcap_exec);
  std::array<uint8_t, 55> tv = {0x00, 0x15, 0x00, 0x33, 0x00, 0x00, 0x04, 0x00, 0x1b, 0x00, 0x08, 0x00, 0x00, 0xf1,
                                0x10, 0x00, 0x00, 0x06, 0x6c, 0x00, 0x52, 0x40, 0x0a, 0x03, 0x80, 0x73, 0x72, 0x73,
                                0x67, 0x6e, 0x62, 0x30, 0x31, 0x00, 0x66, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x07,
                                0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x15, 0x40, 0x01, 0x60};
  ngap_pcap_writer->push_pdu(tv);
}

TEST_F(pcap_ngap_test, write_many_spans)
{
  auto ngap_pcap_writer = create_ngap_pcap("/tmp/ngap_write_many_spans.pcap", pcap_exec);

  uint32_t num_threads         = 10;
  uint32_t num_pdus_per_thread = 100;

  std::vector<std::thread> writer_threads;
  test_logger.info("Start writer_threads");

  for (uint32_t i = 0; i < num_threads; i++) {
    writer_threads.push_back(
        std::thread(write_pcap_ngap_thread_function_spans, ngap_pcap_writer.get(), num_pdus_per_thread));
  }

  test_logger.info("Wait for writer_threads to finish");
  // wait for threads to finish
  for (std::thread& thread : writer_threads) {
    thread.join();
  }
}

TEST_F(pcap_ngap_test, write_many_byte_buffers)
{
  auto ngap_pcap_writer = create_ngap_pcap("/tmp/ngap_write_many_byte_buffers.pcap", pcap_exec);

  uint32_t num_threads         = 10;
  uint32_t num_pdus_per_thread = 100;

  std::vector<std::thread> writer_threads;
  test_logger.info("Start writer_threads");

  for (uint32_t i = 0; i < num_threads; i++) {
    writer_threads.push_back(
        std::thread(write_pcap_ngap_thread_function_byte_buffer, ngap_pcap_writer.get(), num_pdus_per_thread));
  }

  test_logger.info("Wait for writer_threads to finish");
  // wait for threads to finish
  for (std::thread& thread : writer_threads) {
    thread.join();
  }
}

// Write #num_pdus DL MAC NR PDUs using PCAP handle (spans)
void write_pcap_ngap_thread_function_spans(srsran::dlt_pcap* pcap, uint32_t num_pdus)
{
  std::array<uint8_t, 55> tv = {0x00, 0x15, 0x00, 0x33, 0x00, 0x00, 0x04, 0x00, 0x1b, 0x00, 0x08, 0x00, 0x00, 0xf1,
                                0x10, 0x00, 0x00, 0x06, 0x6c, 0x00, 0x52, 0x40, 0x0a, 0x03, 0x80, 0x73, 0x72, 0x73,
                                0x67, 0x6e, 0x62, 0x30, 0x31, 0x00, 0x66, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x07,
                                0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x15, 0x40, 0x01, 0x60};
  for (uint32_t i = 0; i < num_pdus; i++) {
    pcap->push_pdu(tv);
  }

  std::cout << "Finished thread " << std::this_thread::get_id() << "\n";
}

// Write #num_pdus DL MAC NR PDUs using PCAP handle (byte_buffer)
void write_pcap_ngap_thread_function_byte_buffer(srsran::dlt_pcap* pcap, uint32_t num_pdus)
{
  srsran::byte_buffer tv = {0x00, 0x15, 0x00, 0x33, 0x00, 0x00, 0x04, 0x00, 0x1b, 0x00, 0x08, 0x00, 0x00, 0xf1,
                            0x10, 0x00, 0x00, 0x06, 0x6c, 0x00, 0x52, 0x40, 0x0a, 0x03, 0x80, 0x73, 0x72, 0x73,
                            0x67, 0x6e, 0x62, 0x30, 0x31, 0x00, 0x66, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x07,
                            0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x15, 0x40, 0x01, 0x60};
  for (uint32_t i = 0; i < num_pdus; i++) {
    pcap->push_pdu(tv.copy());
  }

  std::cout << "Finished thread " << std::this_thread::get_id() << "\n";
}
