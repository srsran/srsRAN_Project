
#ifndef SRSGNB_MAC_MAC_H
#define SRSGNB_MAC_MAC_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/mac/mac_resource_manager.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/support/async/lazy_task.h"
#include <memory>

namespace srsgnb {

/// FAPI, MaxULPDUsPerSlot. Value is implementation-defined.
constexpr size_t MAX_UL_PDUS_PER_SLOT = 16;

using harq_pid              = uint8_t;
using cqi_report            = uint8_t;
using timing_advance_report = uint16_t;
using rssi_report           = uint16_t;

struct mac_rx_sdu {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  byte_buffer   pdu;
};

/// MAC Received PDU that comprises the list of PDUs of Rx_Data.indication.
struct mac_rx_pdu {
  rnti_t      rnti;
  uint8_t     rapid;
  harq_pid    harq_id;
  byte_buffer pdu;
};
using mac_rx_pdu_list = static_vector<mac_rx_pdu, MAX_UL_PDUS_PER_SLOT>;

/// Rx_Data.indication Message.
struct mac_rx_data_indication {
  slot_point      sl_rx;
  du_cell_index_t cell_index;
  mac_rx_pdu_list pdus;
};

/// CRC.Indication
struct crc_indication_message {
  slot_point sl_rx;
  // TODO: remaining fields.
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

struct logical_channel_addmod {
  lcid_t               lcid;
  mac_sdu_rx_notifier* ul_bearer;
  mac_sdu_tx_builder*  dl_bearer;
};

struct mac_ue_create_request_message {
  du_cell_index_t                     cell_index;
  du_ue_index_t                       ue_index;
  rnti_t                              crnti;
  std::vector<logical_channel_addmod> bearers;
  const byte_buffer*                  ul_ccch_msg;
};

struct mac_ue_create_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

struct mac_ue_reconfiguration_request_message {
  du_cell_index_t                     cell_index;
  du_ue_index_t                       ue_index;
  rnti_t                              crnti;
  std::vector<logical_channel_addmod> bearers_to_addmod;
  std::vector<lcid_t>                 bearers_to_rem;
};

struct mac_ue_reconfiguration_response_message {
  du_ue_index_t ue_index;
  bool          result;
};

struct mac_ue_delete_request_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
};

struct mac_ue_delete_response_message {
  bool result;
};

struct mac_rach_indication {
  du_ue_index_t cell_index;
  slot_point    slot_rx;
  /// Index of the first OFDM Symbol where RACH was detected
  unsigned symbol_index;
  unsigned frequency_index;
  unsigned preamble_id;
  unsigned timing_advance;
};

/// Methods used by MAC to notify events.
class mac_event_notifier
{
public:
  virtual ~mac_event_notifier()                                               = default;
  virtual void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) = 0;
};

/// Interface used to manage the creation/reconfiguration/deletion of UEs in MAC
class mac_configurer
{
public:
  virtual ~mac_configurer() = default;
  virtual lazy_task<mac_ue_create_response_message> ue_create_request(const mac_ue_create_request_message& cfg) = 0;
  virtual lazy_task<mac_ue_reconfiguration_response_message>
                                                    ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& cfg) = 0;
  virtual lazy_task<mac_ue_delete_response_message> ue_delete_request(const mac_ue_delete_request_message& cfg) = 0;
  virtual void                                      flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)  = 0;
};

class mac_rach_handler
{
public:
  virtual ~mac_rach_handler()                                              = default;
  virtual void handle_rach_indication(const mac_rach_indication& rach_ind) = 0;
};

/// Interface used to push Rx Data indications to L2.
class mac_sdu_rx_handler
{
public:
  virtual ~mac_sdu_rx_handler() = default;

  /// This function is called whenever the MAC receives a Rx_Data.indication
  virtual void handle_rx_data_indication(mac_rx_data_indication pdu) = 0;
};

/// Interface used to handle slot indications specific to a cell.
class mac_cell_slot_handler
{
public:
  virtual ~mac_cell_slot_handler()                      = default;
  virtual void handle_slot_indication(slot_point sl_tx) = 0;
};

/// Interface used to handle a MAC cell activation/deactivation.
class mac_cell_manager
{
public:
  virtual ~mac_cell_manager()     = default;
  virtual lazy_task<void> start() = 0;
  virtual lazy_task<void> stop()  = 0;
};

/// Interface to handle feedback information from the PHY.
class mac_control_information_handler
{
public:
  virtual ~mac_control_information_handler() = default;

  virtual void handle_crc(const crc_indication_message& msg) = 0;
};

class mac_interface : public mac_resource_manager,
                      public mac_configurer,
                      public mac_sdu_rx_handler,
                      public mac_control_information_handler
{
public:
  /// Returns handler of PRACHs.
  virtual mac_rach_handler& get_rach_handler() = 0;

  /// Returns handler of slot indications for a cell with provided cell_index.
  virtual mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) = 0;

  /// Returns cell management handle for the provided cell index.
  virtual mac_cell_manager& get_cell_manager(du_cell_index_t cell_index) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_MAC_H
