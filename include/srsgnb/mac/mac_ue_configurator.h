
#ifndef SRSGNB_MAC_UE_CONFIGURATOR_H
#define SRSGNB_MAC_UE_CONFIGURATOR_H

#include "mac_pdu_handler.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

/// Parameters passed to MAC concerning a created logical channel.
struct mac_logical_channel_addmod {
  lcid_t               lcid;
  mac_sdu_rx_notifier* ul_bearer;
  mac_sdu_tx_builder*  dl_bearer;
};

/// Input parameters used to create a UE in the scheduler.
struct mac_ue_create_request_message {
  du_cell_index_t                         cell_index;
  du_ue_index_t                           ue_index;
  rnti_t                                  crnti;
  std::vector<mac_logical_channel_addmod> bearers;
  const byte_buffer*                      ul_ccch_msg;
};

/// Outcome of a MAC UE creation request procedure.
struct mac_ue_create_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

/// Input parameters used to reconfigure a UE in the scheduler.
struct mac_ue_reconfiguration_request_message {
  du_cell_index_t                         cell_index;
  du_ue_index_t                           ue_index;
  rnti_t                                  crnti;
  std::vector<mac_logical_channel_addmod> bearers_to_addmod;
  std::vector<lcid_t>                     bearers_to_rem;
};

struct mac_ue_reconfiguration_response_message {
  du_ue_index_t ue_index;
  bool          result;
};

/// Input parameters used to delete a UE in the scheduler.
struct mac_ue_delete_request_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
};

struct mac_ue_delete_response_message {
  bool result;
};

/// Interface used to manage the creation/reconfiguration/deletion of UEs in MAC
class mac_ue_configurator
{
public:
  virtual ~mac_ue_configurator() = default;
  virtual async_task<mac_ue_create_response_message>
  handle_ue_create_request(const mac_ue_create_request_message& cfg) = 0;
  virtual async_task<mac_ue_reconfiguration_response_message>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& cfg) = 0;
  virtual async_task<mac_ue_delete_response_message>
               handle_ue_delete_request(const mac_ue_delete_request_message& cfg) = 0;
  virtual void flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)         = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UE_CONFIGURATOR_H
