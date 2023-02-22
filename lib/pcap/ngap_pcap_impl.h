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

#pragma once

#include "pcap_file_base.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/executors/task_worker.h"

namespace srsran {

class ngap_pcap_impl : public ngap_pcap
{
public:
  ngap_pcap_impl();
  ~ngap_pcap_impl() override;
  ngap_pcap_impl(const ngap_pcap_impl& other)            = delete;
  ngap_pcap_impl& operator=(const ngap_pcap_impl& other) = delete;
  ngap_pcap_impl(ngap_pcap_impl&& other)                 = delete;
  ngap_pcap_impl& operator=(ngap_pcap_impl&& other)      = delete;

  void open(const char* filename_) override;
  void close() override;
  bool is_write_enabled() override;
  void push_pdu(srsran::byte_buffer pdu) override;
  void push_pdu(srsran::const_span<uint8_t> pdu) override;

private:
  void                 write_pdu(srsran::byte_buffer buf);
  task_worker          worker;
  std::vector<uint8_t> tmp_mem;
  pcap_file_base       writter;
};

} // namespace srsran
