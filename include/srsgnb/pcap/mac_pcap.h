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

#include "srsgnb/pcap/pcap.h"

namespace srsgnb {

constexpr const char* MAC_NR_START_STRING            = "mac-nr";
constexpr uint8_t     MAC_NR_PAYLOAD_TAG             = 0x01;
constexpr uint8_t     MAC_NR_RNTI_TAG                = 0x02;
constexpr uint8_t     MAC_NR_UEID_TAG                = 0x03;
constexpr uint8_t     MAC_NR_FRAME_SUBFRAME_TAG      = 0x04;
constexpr uint8_t     MAC_NR_PHR_TYPE2_OTHERCELL_TAG = 0x05;
constexpr uint8_t     MAC_NR_HARQID                  = 0x06;

/* Context information for every MAC NR PDU that will be logged */
typedef struct {
  uint8_t  radioType;
  uint8_t  direction;
  uint8_t  rntiType;
  uint16_t rnti;
  uint16_t ueid;
  uint8_t  harqid;

  uint8_t phr_type2_othercell;

  uint16_t system_frame_number;
  uint8_t  sub_frame_number;
  uint16_t length;
} mac_nr_context_info;

class mac_pcap : public pcap_file_base
{
public:
  mac_pcap() = default;
  ~mac_pcap();

  void open(const char* filename_);
  void close();
  void write_pdu(mac_nr_context_info& context, srsgnb::const_span<uint8_t> pdu);
};
} // namespace srsgnb

