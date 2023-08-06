/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

  virtual void open(const std::string& filename_)                             = 0;
  virtual void close()                                                        = 0;
  virtual bool is_write_enabled()                                             = 0;
  virtual void push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu) = 0;
  virtual void push_pdu(mac_nr_context_info context, byte_buffer pdu)         = 0;
};

// DLT PCAP values for different layers
constexpr uint16_t PCAP_NGAP_DLT = 152;
constexpr uint16_t PCAP_E1AP_DLT = 153;
constexpr uint16_t PCAP_F1AP_DLT = 154;
constexpr uint16_t PCAP_E2AP_DLT = 155;

/// @brief Interface class for writing a DLT PCAP to a file.
class dlt_pcap
{
public:
  virtual ~dlt_pcap() = default;

  virtual void open(const std::string& filename_) = 0;
  virtual void close()                            = 0;
  virtual bool is_write_enabled()                 = 0;
  virtual void push_pdu(const_span<uint8_t> pdu)  = 0;
  virtual void push_pdu(byte_buffer pdu)          = 0;
};
} // namespace srsran
