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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/mac/mac_constants.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// CQI Value reported.
using cqi_report = uint8_t;

/// RSSI value reported.
using rssi_report = uint16_t;

/// CRC indication for a given PDU.
struct mac_crc_pdu {
  rnti_t                          rnti;
  uint8_t                         rapid;
  uint8_t                         harq_id;
  bool                            tb_crc_success;
  bounded_bitset<MAX_CBS_PER_PDU> cb_crc_status;
  uint16_t                        ul_sinr_metric;
  phy_time_unit                   ta;
  uint16_t                        rssi;
  uint16_t                        rsrp;
};

/// \brief List of UL CRC indications for a given slot and cell.
struct mac_crc_indication_message {
  slot_point                                       sl_rx;
  static_vector<mac_crc_pdu, MAX_UL_PDUS_PER_SLOT> crcs;
};

/// Interface to handle feedback information from the PHY.
class mac_cell_control_information_handler
{
public:
  virtual ~mac_cell_control_information_handler() = default;

  virtual void handle_crc(const mac_crc_indication_message& msg) = 0;
};

} // namespace srsgnb
