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
#include "srsgnb/adt/span.h"
#include <cstdint>

namespace srsgnb {

// Radio Type
constexpr uint8_t PCAP_FDD_RADIO = 1;
constexpr uint8_t PCAP_TDD_RADIO = 2;

// Direction
constexpr uint8_t PCAP_DIRECTION_UPLINK   = 0;
constexpr uint8_t PCAP_DIRECTION_DOWNLINK = 1;

// RNTI types
constexpr uint8_t PCAP_NO_RNTI     = 0; // Used for BCH-BCH
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

// Context information for every MAC NR PDU that will be logged
struct mac_nr_context_info {
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
};

/// @brief Interface class for writing a MAC PCAP to a file.
class mac_pcap
{
public:
  virtual ~mac_pcap() = default;

  virtual void open(const char* filename_)                                    = 0;
  virtual void close()                                                        = 0;
  virtual bool is_write_enabled()                                             = 0;
  virtual void push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu) = 0;
  virtual void push_pdu(mac_nr_context_info context, byte_buffer pdu)         = 0;
};

/// @brief Interface class for writing a NGAP PCAP to a file.
class ngap_pcap
{
public:
  virtual ~ngap_pcap() = default;

  virtual void open(const char* filename_)       = 0;
  virtual void close()                           = 0;
  virtual bool is_write_enabled()                = 0;
  virtual void push_pdu(const_span<uint8_t> pdu) = 0;
  virtual void push_pdu(byte_buffer pdu)         = 0;
};
} // namespace srsgnb
