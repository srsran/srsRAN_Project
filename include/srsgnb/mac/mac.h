
#ifndef SRSGNB_MAC_H
#define SRSGNB_MAC_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/common/id_types.h"

namespace srsgnb {

struct ul_ccch_indication_message {
  du_cell_index_t cell_index;
  rnti_t          crnti;
  byte_buffer     ul_ccch_msg;
};

struct mac_ue_create_request_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          crnti;
};

struct mac_ue_create_request_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

struct mac_ue_reconfiguration_request {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          crnti;
};

struct mac_ue_delete_request {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          crnti;
};

/// Interface used to manage the creation/reconfiguration/deletion of UEs in MAC
class mac_config_interface
{
public:
  virtual void ue_create_request(const mac_ue_create_request_message& cfg)           = 0;
  virtual void ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg) = 0;
  virtual void ue_delete_request(const mac_ue_delete_request& cfg)                   = 0;
};

class mac_northbound_notifier
{
public:
  virtual void push_sdu(rnti_t rnti, lcid_t lcid, byte_buffer pdu) = 0;
};

class mac_southbound_interface
{
public:
  virtual void push_ul_pdu(rnti_t rnti, du_cell_index_t cell_index, byte_buffer pdu) = 0;
};

class mac_interface : public mac_config_interface, public mac_southbound_interface
{
public:
  virtual ~mac_interface() = default;
};

class mac_config_notifier
{
public:
  virtual void ue_create_request_complete(const mac_ue_create_request_response_message& resp) = 0;
  virtual void ue_reconfiguration_complete()                                                  = 0;
  virtual void ue_delete_complete()                                                           = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_H
