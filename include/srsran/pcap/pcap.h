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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/span.h"
#include <cstdint>

namespace srsran {

class task_executor;

// DLT PCAP values for different layers
constexpr uint16_t PCAP_NGAP_DLT = 152;
constexpr uint16_t PCAP_E1AP_DLT = 153;
constexpr uint16_t PCAP_F1AP_DLT = 154;
constexpr uint16_t PCAP_E2AP_DLT = 155;
constexpr uint16_t PCAP_GTPU_DLT = 156;

/// @brief Interface class for writing a DLT PCAP to a file.
class dlt_pcap
{
public:
  virtual ~dlt_pcap() = default;

  virtual void close()                           = 0;
  virtual bool is_write_enabled() const          = 0;
  virtual void push_pdu(const_span<uint8_t> pdu) = 0;
  virtual void push_pdu(byte_buffer pdu)         = 0;
};

/// \brief Creates a generic DLT PCAP message sink that writes the incoming PDUs to a file.
///
/// If the provided filename is empty, the returned pcap writer will not be enabled.
std::unique_ptr<dlt_pcap>
create_dlt_pcap(unsigned dlt, const std::string& layer_name, const std::string& filename, task_executor* backend_exec);

/// Null sink for DLT pcap messages. This is useful in unit tests and when the application disables pcaps.
class null_dlt_pcap : public dlt_pcap
{
public:
  void close() override {}
  bool is_write_enabled() const override { return false; }
  void push_pdu(const_span<uint8_t> pdu) override {}
  void push_pdu(byte_buffer pdu) override {}
};

} // namespace srsran
