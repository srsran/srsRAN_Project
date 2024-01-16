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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/span.h"
#include <cstdint>

namespace srsran {

class task_executor;

/// \brief Interface class for writing a DLT PCAP to a file.
class dlt_pcap
{
public:
  virtual ~dlt_pcap() = default;

  virtual void close()                           = 0;
  virtual bool is_write_enabled() const          = 0;
  virtual void push_pdu(const_span<uint8_t> pdu) = 0;
  virtual void push_pdu(byte_buffer pdu)         = 0;
};

/// \brief Creates a layer DLT PCAP sink that writes the incoming PDUs to a pcap file.
std::unique_ptr<dlt_pcap> create_null_dlt_pcap();
std::unique_ptr<dlt_pcap> create_ngap_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_f1ap_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_e1ap_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_gtpu_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_e2ap_pcap(const std::string& filename, task_executor& backend_exec);

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
