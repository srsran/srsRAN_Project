/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/pcap/pcap.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

bool pcap_file_base::dlt_pcap_open(uint32_t dlt_, const char* filename_)
{
  if (pcap_fd != nullptr) {
    logger.error("PCAP {} already open", filename);
    return false;
  }

  dlt = dlt_;
  filename = filename_;

  pcap_hdr_t file_header = {
      0xa1b2c3d4, /// magic number
      2,
      4,     /// version number is 2.4
      0,     /// timezone
      0,     /// sigfigs - apparently all tools do this
      65535, /// snaplen - this should be long enough
      dlt    /// Data Link Type (DLT).  Set as unused value 147 for now
  };

  pcap_fd = fopen(filename.c_str(), "w");
  if (pcap_fd == NULL) {
    logger.error("Failed to open file {} for writing\n", filename);
    return false;
  }

  fwrite(&file_header, sizeof(pcap_hdr_t), 1, pcap_fd);

  write_enabled = true;

  return true;
}

void pcap_file_base::dlt_pcap_close()
{
  if (pcap_fd) {
    logger.info("Saving PCAP file (DLT={}) to {}", dlt, filename.c_str());
    write_enabled = false;
    fclose(pcap_fd);
    pcap_fd = nullptr;
  }
}

void pcap_file_base::write_pcap_header(uint32_t length)
{
  pcaprec_hdr_t packet_header = {};

  // PCAP header
  struct timeval t;
  gettimeofday(&t, NULL);
  packet_header.ts_sec   = t.tv_sec;
  packet_header.ts_usec  = t.tv_usec;
  packet_header.incl_len = length;
  packet_header.orig_len = length;

  fwrite(&packet_header, sizeof(pcaprec_hdr_t), 1, pcap_fd);
}

void pcap_file_base::write_pcap_pdu(srsgnb::const_span<uint8_t> pdu)
{
  if (write_enabled) {
    fwrite(pdu.data(), 1, pdu.size_bytes(), pcap_fd);
  }
}

bool pcap_file_base::is_write_enabled()
{
  return write_enabled;
}