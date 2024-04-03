/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  explicit mac_pcap_impl(const std::string& filename_, mac_pcap_type type, task_executor& backend_exec);

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
  srslog::basic_logger& logger;
  std::string           filename;
  mac_pcap_type         type;
  backend_pcap_writer   writer;
};

} // namespace srsran
