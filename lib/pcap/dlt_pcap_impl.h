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

#include "pcap_file_base.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/executors/task_worker.h"

namespace srsran {

class dlt_pcap_impl : public dlt_pcap
{
public:
  dlt_pcap_impl(unsigned dlt_, std::string layer_name);
  ~dlt_pcap_impl() override;
  dlt_pcap_impl(const dlt_pcap_impl& other)            = delete;
  dlt_pcap_impl& operator=(const dlt_pcap_impl& other) = delete;
  dlt_pcap_impl(dlt_pcap_impl&& other)                 = delete;
  dlt_pcap_impl& operator=(dlt_pcap_impl&& other)      = delete;

  void open(const std::string& filename_) override;
  void close() override;
  bool is_write_enabled() override;
  void push_pdu(srsran::byte_buffer pdu) override;
  void push_pdu(srsran::const_span<uint8_t> pdu) override;

private:
  unsigned             dlt;
  void                 write_pdu(srsran::byte_buffer buf);
  task_worker          worker;
  std::vector<uint8_t> tmp_mem;
  pcap_file_base       writter;
};

} // namespace srsran
