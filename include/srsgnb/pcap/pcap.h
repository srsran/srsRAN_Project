/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/srslog/srslog.h"
#include <fstream>
#include <string>

#define PCAP_CONTEXT_HEADER_MAX 256

#define UDP_DLT 149 // UDP needs to be selected as protocol

constexpr uint16_t pcap_max_len = 2000;

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
class pcap_file_base
{
public:
  pcap_file_base() : logger(srslog::fetch_basic_logger("PCAP")){};
  ~pcap_file_base()                                      = default;
  pcap_file_base(const pcap_file_base& other)            = delete;
  pcap_file_base& operator=(const pcap_file_base& other) = delete;
  pcap_file_base(pcap_file_base&& other)                 = delete;
  pcap_file_base& operator=(pcap_file_base&& other)      = delete;

  bool is_write_enabled();

protected:
  bool dlt_pcap_open(uint32_t dlt, const char* filename);
  void dlt_pcap_close();
  void write_pcap_header(uint32_t length);
  void write_pcap_pdu(srsgnb::const_span<uint8_t> pdu);

private:
  srslog::basic_logger& logger;
  bool                  write_enabled = false;
  std::ofstream         pcap_fstream;
  std::string           filename;
  uint32_t              dlt = 0;
};
