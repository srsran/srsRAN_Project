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
#include "srsran/srslog/srslog.h"
#include <fstream>
#include <string>

#define PCAP_CONTEXT_HEADER_MAX 256

namespace srsran {

/// This structure gets written to the start of the file
struct pcap_hdr_t {
  unsigned int   magic_number;  /// magic number
  unsigned short version_major; /// major version number
  unsigned short version_minor; /// minor version number
  unsigned int   thiszone;      /// GMT to local correction
  unsigned int   sigfigs;       /// accuracy of timestamps
  unsigned int   snaplen;       /// max length of captured packets, in octets
  unsigned int   network;       /// data link type
};

/// This structure precedes each packet
struct pcaprec_hdr_t {
  unsigned int ts_sec;   /// timestamp seconds
  unsigned int ts_usec;  /// timestamp microseconds
  unsigned int incl_len; /// number of octets of packet saved in file
  unsigned int orig_len; /// actual length of packet
};

/// @brief Base class for PCAP writing to files.
/// The class in *not* thread-safe. Proper protection from multiple threads
/// needs to be implemented by the user of the class.
class pcap_file_writer
{
public:
  pcap_file_writer();
  ~pcap_file_writer();
  pcap_file_writer(const pcap_file_writer& other)            = delete;
  pcap_file_writer& operator=(const pcap_file_writer& other) = delete;
  pcap_file_writer(pcap_file_writer&& other)                 = delete;
  pcap_file_writer& operator=(pcap_file_writer&& other)      = delete;

  bool is_write_enabled() const { return pcap_fstream.is_open(); }

  bool open(uint32_t dlt, const std::string& filename);
  void close();
  void write_pdu_header(uint32_t length);
  void write_pdu(srsran::const_span<uint8_t> pdu);
  void write_pdu(const byte_buffer& pdu);

private:
  srslog::basic_logger& logger;
  std::ofstream         pcap_fstream;
  std::string           filename;
  uint32_t              dlt = 0;
};

} // namespace srsran
