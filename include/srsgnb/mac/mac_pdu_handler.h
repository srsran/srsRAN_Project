
#ifndef SRSGNB_MAC_PDU_HANDLER_H
#define SRSGNB_MAC_PDU_HANDLER_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Maximum number of PDUs per slot. This value is implementation-defined.
/// \remark It should match FAPI MaxULPDUsPerSlot.
constexpr size_t MAX_UL_PDUS_PER_SLOT = 16;

/// HARQ process ID value.
using harq_pid_value = uint8_t;

/// MAC Received PDU that comprises the list of PDUs of Rx_Data.indication.
struct mac_rx_pdu {
  rnti_t         rnti;
  uint8_t        rapid;
  harq_pid_value harq_id;
  byte_buffer    pdu;
};
using mac_rx_pdu_list = static_vector<mac_rx_pdu, MAX_UL_PDUS_PER_SLOT>;

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

struct mac_rx_sdu {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  byte_buffer   pdu;
};

/// This interface notifies to upper layers the reception of new SDUs over a logical channel.
class mac_sdu_rx_notifier
{
public:
  virtual ~mac_sdu_rx_notifier() = default;

  /// This callback is invoked on each received SDU.
  virtual void on_new_sdu(mac_rx_sdu sdu) = 0;
};

/// This interface represents the entry point of a logical channel in the MAC layer.
class mac_sdu_tx_builder
{
public:
  virtual ~mac_sdu_tx_builder()                = default;
  virtual void on_new_tx_sdu(byte_buffer& pdu) = 0;
};

/// Used to indicate UL CCCH message arrival.
struct ul_ccch_indication_message {
  du_cell_index_t cell_index;
  rnti_t          crnti;
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

} // namespace srsgnb

#endif // SRSGNB_MAC_PDU_HANDLER_H
