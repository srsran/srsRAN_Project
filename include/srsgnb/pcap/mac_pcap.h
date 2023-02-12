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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/pcap/pcap.h"
#include "srsgnb/support/executors/task_worker.h"

namespace srsgnb {

// PCAP tags
constexpr const char* MAC_NR_START_STRING            = "mac-nr";
constexpr uint8_t     MAC_NR_PAYLOAD_TAG             = 0x01;
constexpr uint8_t     MAC_NR_RNTI_TAG                = 0x02;
constexpr uint8_t     MAC_NR_UEID_TAG                = 0x03;
constexpr uint8_t     MAC_NR_FRAME_SUBFRAME_TAG      = 0x04;
constexpr uint8_t     MAC_NR_PHR_TYPE2_OTHERCELL_TAG = 0x05;
constexpr uint8_t     MAC_NR_HARQID                  = 0x06;

// Radio Type
constexpr uint8_t PCAP_FDD_RADIO = 1;
constexpr uint8_t PCAP_TDD_RADIO = 2;

// Direction
constexpr uint8_t PCAP_DIRECTION_UPLINK   = 0;
constexpr uint8_t PCAP_DIRECTION_DOWNLINK = 1;

// RNTI types
constexpr uint8_t PCAP_NO_RNTI     = 0; /* Used for BCH-BCH */
constexpr uint8_t PCAP_P_RNTI      = 1;
constexpr uint8_t PCAP_RA_RNTI     = 2;
constexpr uint8_t PCAP_C_RNTI      = 3;
constexpr uint8_t PCAP_SI_RNTI     = 4;
constexpr uint8_t PCAP_SPS_RNTI    = 5;
constexpr uint8_t PCAP_M_RNTI      = 6;
constexpr uint8_t PCAP_SL_BCH_RNTI = 7;
constexpr uint8_t PCAP_SL_RNTI     = 8;
constexpr uint8_t PCAP_SC_RNTI     = 9;
constexpr uint8_t PCAP_G_RNTI      = 10;

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
  mac_pcap() : worker("PCAP", 256) {}
  ~mac_pcap();

  task_worker worker;
  void        open(const char* filename_);
  void        close();
  void        push_pdu(const mac_nr_context_info& context, const_span<uint8_t> pdu);
  void        push_pdu(const mac_nr_context_info& context, byte_buffer pdu);

private:
  void write_pdu(const mac_nr_context_info& context, srsgnb::const_span<uint8_t> pdu);
};
} // namespace srsgnb

