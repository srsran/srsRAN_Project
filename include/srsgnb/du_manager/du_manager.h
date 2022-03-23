
#ifndef SRSGNB_DU_MANAGER_H
#define SRSGNB_DU_MANAGER_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include <string>

namespace srsgnb {

struct rlc_ue_create_response_message;
struct rlc_ue_delete_response_message;
struct rlc_ue_reconfiguration_response_message;
struct mac_ue_create_request_response_message;
struct mac_ue_reconfiguration_response_message;
struct mac_ue_delete_response_message;
struct ul_ccch_indication_message;

class du_manager_interface_rlc
{
public:
  virtual ~du_manager_interface_rlc()                                                                      = default;
  virtual void handle_rlc_ue_reconfiguration_response(const rlc_ue_reconfiguration_response_message& resp) = 0;
  virtual void handle_rlc_ue_delete_response(const rlc_ue_delete_response_message& resp)                   = 0;
};

struct du_ue_create_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          crnti;
  struct logical_channel_to_addmod {
    lcid_t lcid;
    // other params
  };
  std::vector<logical_channel_to_addmod> logical_channels_to_add;
};

struct du_ue_delete_message {
  du_ue_index_t ue_index;
};

struct du_ue_create_response_message {
  du_ue_index_t ue_index;
  bool          result;
};

class du_manager_ccch_indicator
{
public:
  virtual ~du_manager_ccch_indicator()                                          = default;
  virtual void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) = 0;
};

class du_manager_interface_query
{
public:
  virtual ~du_manager_interface_query() = default;
  virtual size_t nof_ues()              = 0;
};

class du_manager_interface : public du_manager_interface_rlc,
                             public du_manager_interface_query,
                             public du_manager_ccch_indicator
{
public:
  virtual ~du_manager_interface() = default;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_H
