
#ifndef SRSGNB_MAC_H
#define SRSGNB_MAC_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/support/async/async_task.h"
#include <memory>

namespace srsgnb {

struct mac_ul_sdu {
  rnti_t      rnti;
  lcid_t      lcid;
  byte_buffer pdu;
};

struct mac_rx_data_indication {
  rnti_t          rnti;
  lcid_t          lcid;
  du_cell_index_t cell_index;
  byte_buffer     pdu;
};

class mac_ul_sdu_notifier
{
public:
  virtual ~mac_ul_sdu_notifier()         = default;
  virtual void on_ul_sdu(mac_ul_sdu sdu) = 0;
};

class mac_dl_sdu_builder
{
public:
  virtual ~mac_dl_sdu_builder()                = default;
  virtual void on_dl_mac_sdu(byte_buffer& pdu) = 0;
};

struct ul_ccch_indication_message {
  du_cell_index_t cell_index;
  rnti_t          crnti;
  byte_buffer     ul_ccch_msg;
};

struct logical_channel_addmod {
  lcid_t               lcid;
  mac_ul_sdu_notifier* ul_bearer;
  mac_dl_sdu_builder*  dl_bearer;
};

struct mac_ue_create_request_message {
  du_cell_index_t                     cell_index;
  du_ue_index_t                       ue_index;
  rnti_t                              crnti;
  std::vector<logical_channel_addmod> bearers;
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
  du_ue_index_t ue_index;
  bool          result;
};

/// Interface used to manage the creation/reconfiguration/deletion of UEs in MAC
class mac_configurer
{
public:
  virtual ~mac_configurer() = default;
  virtual async_task<mac_ue_create_response_message> ue_create_request(const mac_ue_create_request_message& cfg) = 0;
  virtual async_task<mac_ue_reconfiguration_response_message>
                                                     ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& cfg) = 0;
  virtual async_task<mac_ue_delete_response_message> ue_delete_request(const mac_ue_delete_request_message& cfg) = 0;
};

class mac_southbound_interface
{
public:
  virtual ~mac_southbound_interface()                                        = default;
  virtual void push_rx_data_indication(mac_rx_data_indication pdu)           = 0;
  virtual void slot_indication(slot_point sl_tx, du_cell_index_t cell_index) = 0;
};

class mac_interface : public mac_southbound_interface, public mac_configurer
{};

} // namespace srsgnb

#endif // SRSGNB_MAC_H
