
#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// HARQ process ID value.
using harq_pid_value = uint8_t;

/// MAC Received PDU that comprises the list of PDUs of Rx_Data.indication.
struct mac_rx_pdu {
  rnti_t         rnti;
  uint8_t        rapid;
  harq_pid_value harq_id;
  byte_buffer    pdu;
};
using mac_rx_pdu_list = static_vector<mac_rx_pdu, MAX_PUSCH_PDUS_PER_SLOT>;

/// Rx_Data.indication Message.
struct mac_rx_data_indication {
  slot_point      sl_rx;
  du_cell_index_t cell_index;
  mac_rx_pdu_list pdus;
};

/// Interface used to push Rx Data indications to L2.
class mac_pdu_handler
{
public:
  virtual ~mac_pdu_handler() = default;

  /// This function is called whenever the MAC receives a Rx_Data.indication
  virtual void handle_rx_data_indication(mac_rx_data_indication pdu) = 0;
};

/// Used to indicate UL CCCH message arrival.
struct ul_ccch_indication_message {
  du_cell_index_t cell_index;
  rnti_t          tc_rnti;
  slot_point      slot_rx;
  byte_buffer     subpdu;
};

/// Methods used by MAC to notify events.
class mac_ul_ccch_notifier
{
public:
  virtual ~mac_ul_ccch_notifier()                                             = default;
  virtual void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) = 0;
};

} // namespace srsran
