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

#include "srsgnb/sdap/sdap.h"
#include "srsgnb/sdap/sdap_packet_procedures.h"
#include "srsgnb/sdap/sdap_rx_pdu_unpack.h"
#include <cstdio>

namespace srsgnb {

namespace srs_cu_up {

/// The idea is to have an object to handle each specific procedure specified in the TS.
/// This class would implement the Receive operation procedure of the data transfer category (TS 38.323 5.2.2).
class sdap_ul_packet_procedure : public sdap_packet_procedures
{
  /// In reality, as the interfaces grow, this would become a reference or ptr and ownership would be managed from a
  /// different class. For simplicity, we keep it this way for now.
  /// This is important as the dependency with f1u_packet_procedures interface would eventually disappear, although it
  /// is an interface and not a concrete class the less dependencies the better.

  /// Both dependencies are interfaces and they are at the same or in a higher abstraction layer, conforming to the
  /// clean architecture paradigm.
  std::unique_ptr<sdap_rx_pdu_unpack> entity;
  sdap_sdu_rx_notifier&               listener;

public:
  sdap_ul_packet_procedure(std::unique_ptr<sdap_rx_pdu_unpack> entity_, sdap_sdu_rx_notifier& listener_) :
    entity(std::move(entity_)), listener(listener_)
  {
  }

  void handle_ul_data_packet(byte_buffer& data) override
  {
    printf("[SDAP-UL-PKT-PROCEDURE] Received a new data packet of size = %u, performing UL packet procedure\n",
           (unsigned)data.length());
    printf("[SDAP-UL-PKT-PROCEDURE] Step 1: remove SDAP header\n");
    entity->decapsulate(data);
    printf("[SDAP-UL-PKT-PROCEDURE] UL packet procedure finished, delivering packet to upper layer\n");
    listener.on_new_sdu(std::move(data));
  }
};

} // namespace srs_cu_up

} // namespace srsgnb
