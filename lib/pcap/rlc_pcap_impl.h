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

#pragma once

#include "backend_pcap_writer.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/pcap/rlc_pcap.h"

namespace srsran {

class rlc_pcap_impl final : public rlc_pcap
{
public:
  rlc_pcap_impl(const std::string& filename, bool capture_srb, bool capture_drb, task_executor& backend_exec);
  ~rlc_pcap_impl() override;
  rlc_pcap_impl(const rlc_pcap_impl& other)            = delete;
  rlc_pcap_impl& operator=(const rlc_pcap_impl& other) = delete;
  rlc_pcap_impl(rlc_pcap_impl&& other)                 = delete;
  rlc_pcap_impl& operator=(rlc_pcap_impl&& other)      = delete;

  void close() override;
  bool is_write_enabled() const override { return writer.is_write_enabled(); }

  void push_pdu(const pcap_rlc_pdu_context& context, const span<uint8_t> pdu) override;
  void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_slice& pdu) override;

private:
  bool srb_enabled = true;
  bool drb_enabled = true;

  srslog::basic_logger& logger;

  backend_pcap_writer writer;
};
} // namespace srsran
