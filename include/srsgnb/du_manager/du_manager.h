
#ifndef SRSGNB_F1AP_LOWER_INTERFACES_H
#define SRSGNB_F1AP_LOWER_INTERFACES_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include <string>

namespace srsgnb {

struct rlc_ue_create_response_message;
struct rlc_ue_delete_response_message;
struct rlc_ue_reconfiguration_response_message;
struct mac_ue_create_request_response_message;
struct mac_ue_delete_response_message;

class du_manager_interface_rlc
{
public:
  virtual void handle_rlc_ue_create_response(const rlc_ue_create_response_message& resp)                   = 0;
  virtual void handle_rlc_ue_reconfiguration_response(const rlc_ue_reconfiguration_response_message& resp) = 0;
  virtual void handle_rlc_ue_delete_response(const rlc_ue_delete_response_message& resp)                   = 0;
};

class du_manager_interface_mac
{
public:
  virtual void handle_mac_ue_create_response(const mac_ue_create_request_response_message& resp) = 0;
  virtual void handle_mac_ue_reconfiguration_response()                                          = 0;
  virtual void handle_mac_ue_delete_response(const mac_ue_delete_response_message& resp)         = 0;
  //  virtual void mac_rach_resource_response()      = 0;
  //  virtual void ue_reset_response()               = 0;
  //  virtual void sync_status_indication()          = 0;
};

struct du_ue_create_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          crnti;
};

struct du_ue_create_response_message {
  du_ue_index_t ue_index;
};

class du_manager_config_notifier
{
public:
  virtual void on_du_ue_create_response(const du_ue_create_response_message& resp) = 0;
};

class du_manager_interface_f1ap
{
public:
  virtual void ue_create(const du_ue_create_message& msg) = 0;
};

class du_manager_interface_query
{
public:
  virtual std::string get_ues() = 0;
};

class du_manager_interface : public du_manager_interface_rlc,
                             public du_manager_interface_mac,
                             public du_manager_interface_f1ap,
                             public du_manager_interface_query
{
public:
  virtual ~du_manager_interface() = default;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_LOWER_INTERFACES_H
