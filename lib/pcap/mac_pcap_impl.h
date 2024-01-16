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
#include "srsran/pcap/mac_pcap.h"
#include "srsran/support/executors/task_worker.h"

namespace srsran {

// PCAP tags
constexpr const char* MAC_NR_START_STRING            = "mac-nr";
constexpr uint8_t     MAC_NR_PAYLOAD_TAG             = 0x01;
constexpr uint8_t     MAC_NR_RNTI_TAG                = 0x02;
constexpr uint8_t     MAC_NR_UEID_TAG                = 0x03;
constexpr uint8_t     MAC_NR_FRAME_SUBFRAME_TAG      = 0x04;
constexpr uint8_t     MAC_NR_PHR_TYPE2_OTHERCELL_TAG = 0x05;
constexpr uint8_t     MAC_NR_HARQID                  = 0x06;

class mac_pcap_impl final : public mac_pcap
{
public:
  explicit mac_pcap_impl(const std::string& filename, mac_pcap_type type, task_executor& backend_exec);
  ~mac_pcap_impl() override;
  mac_pcap_impl(const mac_pcap_impl& other)            = delete;
  mac_pcap_impl& operator=(const mac_pcap_impl& other) = delete;
  mac_pcap_impl(mac_pcap_impl&& other)                 = delete;
  mac_pcap_impl& operator=(mac_pcap_impl&& other)      = delete;

  void close() override;
  bool is_write_enabled() const override { return writer.is_write_enabled(); }
  void push_pdu(const mac_nr_context_info& context, const_span<uint8_t> pdu) override;
  void push_pdu(const mac_nr_context_info& context, byte_buffer pdu) override;

private:
  byte_buffer pack_context(const mac_nr_context_info& context, const byte_buffer& pdu) const;

  mac_pcap_type type;

  srslog::basic_logger& logger;

  backend_pcap_writer writer;
};

} // namespace srsran
