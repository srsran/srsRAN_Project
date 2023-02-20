/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/pcap/ngap_pcap_impl.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

void write_pcap_ngap_thread_function_byte_buffer(srsran::ngap_pcap* pcap, uint32_t num_pdus);
void write_pcap_ngap_thread_function_spans(srsran::ngap_pcap* pcap, uint32_t num_pdus);

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
    ngap_pcap_writer.close();
    // flush logger after each test
    srslog::flush();
  }

  srsran::ngap_pcap_impl ngap_pcap_writer;
  srslog::basic_logger&  test_logger = srslog::fetch_basic_logger("TEST");
};

TEST_F(pcap_ngap_test, write_pdu)
{
  ngap_pcap_writer.open("ngap_write_pdu.pcap");
  std::array<uint8_t, 55> tv = {0x00, 0x15, 0x00, 0x33, 0x00, 0x00, 0x04, 0x00, 0x1b, 0x00, 0x08, 0x00, 0x00, 0xf1,
                                0x10, 0x00, 0x00, 0x06, 0x6c, 0x00, 0x52, 0x40, 0x0a, 0x03, 0x80, 0x73, 0x72, 0x73,
                                0x67, 0x6e, 0x62, 0x30, 0x31, 0x00, 0x66, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x07,
                                0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x15, 0x40, 0x01, 0x60};
  ngap_pcap_writer.push_pdu(tv);
}

TEST_F(pcap_ngap_test, write_many_spans)
{
  ngap_pcap_writer.open("ngap_write_many_spans.pcap");

  uint32_t num_threads         = 10;
  uint32_t num_pdus_per_thread = 100;

  std::vector<std::thread> writer_threads;
  test_logger.info("Start writer_threads");

  for (uint32_t i = 0; i < num_threads; i++) {
    writer_threads.push_back(
        std::thread(write_pcap_ngap_thread_function_spans, &ngap_pcap_writer, num_pdus_per_thread));
  }

  test_logger.info("Wait for writer_threads to finish");
  // wait for threads to finish
  for (std::thread& thread : writer_threads) {
    thread.join();
  }

  test_logger.info("Close PCAP handle");
  ngap_pcap_writer.close();
}

TEST_F(pcap_ngap_test, write_many_byte_buffers)
{
  ngap_pcap_writer.open("ngap_write_many_byte_buffers.pcap");

  uint32_t num_threads         = 10;
  uint32_t num_pdus_per_thread = 100;

  std::vector<std::thread> writer_threads;
  test_logger.info("Start writer_threads");

  for (uint32_t i = 0; i < num_threads; i++) {
    writer_threads.push_back(
        std::thread(write_pcap_ngap_thread_function_byte_buffer, &ngap_pcap_writer, num_pdus_per_thread));
  }

  test_logger.info("Wait for writer_threads to finish");
  // wait for threads to finish
  for (std::thread& thread : writer_threads) {
    thread.join();
  }

  test_logger.info("Close PCAP handle");
  ngap_pcap_writer.close();
}

// Write #num_pdus DL MAC NR PDUs using PCAP handle (spans)
void write_pcap_ngap_thread_function_spans(srsran::ngap_pcap* pcap, uint32_t num_pdus)
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
void write_pcap_ngap_thread_function_byte_buffer(srsran::ngap_pcap* pcap, uint32_t num_pdus)
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
