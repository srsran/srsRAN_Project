/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pcap_file_writer.h"
#include <sys/time.h>

using namespace srsran;

pcap_file_writer::pcap_file_writer() : logger(srslog::fetch_basic_logger("ALL")) {}

pcap_file_writer::~pcap_file_writer()
{
  close();
}

bool pcap_file_writer::open(uint32_t dlt_, const std::string& filename_)
{
  if (pcap_fstream.is_open()) {
    logger.error("Failed to open PCAP file \"{}\". Cause: File is already open", filename);
    return false;
  }

  dlt      = dlt_;
  filename = filename_;

  pcap_hdr_t file_header = {
      0xa1b2c3d4,                           // magic number
      2,                                    // version_major is 2
      4,                                    // version_minor is 4 (together: 2.4)
      0,                                    // timezone
      0,                                    // sigfigs - apparently all tools do this
      std::numeric_limits<uint32_t>::max(), // snaplen - this should be long enough
      dlt                                   // Data Link Type (DLT). Set as unused value 147 for now
  };

  pcap_fstream.open(filename.c_str(), std::ios::out | std::ios::binary);
  if (pcap_fstream.fail()) {
    logger.error("Failed to open file {} for writing: {}", filename, ::strerror(errno));
    return false;
  }
  logger.debug("Opened file {} for writing. DLT={}", filename, dlt);

  pcap_fstream.write((char*)&file_header, sizeof(file_header));
  if (pcap_fstream.fail()) {
    logger.error("Failed to write to PCAP: {}", ::strerror(errno));
    return false;
  }

  return true;
}

void pcap_file_writer::flush()
{
  if (pcap_fstream.is_open()) {
    logger.debug("Flushing PCAP (DLT={})", dlt);
    pcap_fstream.flush();
    return;
  }
  logger.info("Failed to flush closed PCAP (DLT={})", dlt);
}

uint16_t get_dissector_name_size(const std::string& dissector)
{
  uint16_t dissector_size = dissector.size();
  if (dissector_size == 0) {
    return 0;
  }

  // Size of dissector name option.
  uint16_t size_remainder = dissector_size % 4;
  return dissector_size - size_remainder + 4; // round to multiple of 4.
}

uint16_t get_export_pdu_metadata_length(const std::string& dissector)
{
  uint16_t dissector_size = dissector.size();
  if (dissector_size == 0) {
    fmt::println("no dissector!!{}", dissector_size);
    return 0;
  }

  uint16_t dissector_name_size      = get_dissector_name_size(dissector);
  uint16_t dissector_name_opts_size = dissector_name_size + 4;

  return dissector_name_opts_size + EXP_PDU_LENGTH_END_OF_OPT;
}

void pcap_file_writer::close()
{
  flush();
  if (pcap_fstream.is_open()) {
    logger.debug("Saving PCAP (DLT={}) to \"{}\"", dlt, filename);
    pcap_fstream.close();
    logger.info("Saved PCAP (DLT={}) to \"{}\" and closed", dlt, filename);
  }
  logger.info("Failed to close already closed PCAP (DLT={})", dlt);
}

void pcap_file_writer::write_pdu_header(uint32_t length, const std::string& dissector)
{
  pcaprec_hdr_t packet_header = {};

  uint16_t exp_pdu_sz = get_export_pdu_metadata_length(dissector);
  fmt::println("{}", exp_pdu_sz);
  // PCAP header
  struct timeval t = {};
  gettimeofday(&t, nullptr);
  packet_header.ts_sec   = t.tv_sec;
  packet_header.ts_usec  = t.tv_usec;
  packet_header.incl_len = length + exp_pdu_sz;
  packet_header.orig_len = length + exp_pdu_sz;

  pcap_fstream.write((char*)&packet_header, sizeof(packet_header));
  if (pcap_fstream.fail()) {
    logger.error("Failed to write to PCAP: {}", ::strerror(errno));
    return;
  }
}

void pcap_file_writer::write_exported_pdu_header(const std::string& dissector)
{
  if (not is_write_enabled()) {
    return;
  }
  uint16_t dissector_len = dissector.size();
  if (dissector_len == 0) {
    return;
  }

  if (not pcap_fstream.write((char*)&EXP_PDU_TAG_DISSECTOR_NAME, sizeof(uint16_t))) {
    logger.error("Failed to write packet header to pcap: {}", strerror(errno));
    return;
  }

  uint16_t size_remainder = dissector_len % 4;
  uint16_t opt_size       = ntohs(dissector_len - size_remainder + 4); // round to multiple of 4.
  if (not pcap_fstream.write((char*)&opt_size, sizeof(uint16_t))) {
    logger.error("Failed to write packet header to pcap: {}", strerror(errno));
    return;
  }

  // TODO use padding if required
  if (not pcap_fstream.write(dissector.data(), dissector.size())) {
    logger.error("Failed to write packet header to pcap: {}", strerror(errno));
    return;
  }

  if (dissector.size() == 3) {
    if (not pcap_fstream.write("\0", 1)) {
      logger.error("Failed to write packet header to pcap: {}", strerror(errno));
      return;
    }
  }

  if (not pcap_fstream.write((char*)&EXP_PDU_TAG_END_OF_OPT, sizeof(uint32_t))) {
    logger.error("Failed to write packet header to pcap: {}", strerror(errno));
    return;
  }
}

void pcap_file_writer::write_pdu(srsran::const_span<uint8_t> pdu)
{
  if (not is_write_enabled()) {
    return;
  }

  pcap_fstream.write((char*)pdu.data(), pdu.size());
  if (pcap_fstream.fail()) {
    logger.error("Failed to write to PCAP: {}", ::strerror(errno));
    return;
  }
}

void pcap_file_writer::write_pdu(const byte_buffer& pdu)
{
  if (not is_write_enabled()) {
    return;
  }

  for (span<const uint8_t> seg : pdu.segments()) {
    pcap_fstream.write((char*)seg.data(), seg.size());
    if (pcap_fstream.fail()) {
      logger.error("Failed to write to PCAP: {}", ::strerror(errno));
      return;
    }
  }
}
