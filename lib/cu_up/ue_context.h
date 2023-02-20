/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdu_session_manager.h"
#include "pdu_session_manager_impl.h"
#include "srsgnb/cu_up/cu_up_types.h"
#include "srsgnb/f1u/cu_up/f1u_gateway.h"
#include <map>

namespace srsran {
namespace srs_cu_up {

/// \brief Context for a UE within the CU-UP with storage for all active PDU sessions.
class ue_context : public pdu_session_manager_ctrl
{
public:
  ue_context(ue_index_t                           index_,
             network_interface_config&            net_config_,
             srslog::basic_logger&                logger_,
             timer_manager&                       timers_,
             f1u_cu_up_gateway&                   f1u_gw_,
             gtpu_tunnel_tx_upper_layer_notifier& gtpu_tx_notifier_,
             gtpu_demux_ctrl&                     gtpu_rx_demux_) :
    index(index_),
    pdu_session_manager(index, net_config_, logger_, timers_, f1u_gw_, gtpu_tx_notifier_, gtpu_rx_demux_){};
  ~ue_context() = default;

  // pdu_session_manager_ctrl
  pdu_session_setup_result setup_pdu_session(const e1ap_pdu_session_res_to_setup_item& session) override
  {
    return pdu_session_manager.setup_pdu_session(session);
  }
  pdu_session_modification_result
  modify_pdu_session(const asn1::e1ap::pdu_session_res_to_modify_item_s& session) override
  {
    return pdu_session_manager.modify_pdu_session(session);
  }
  void remove_pdu_session(pdu_session_id_t pdu_session_id) override
  {
    pdu_session_manager.remove_pdu_session(pdu_session_id);
  }
  size_t get_nof_pdu_sessions() override { return pdu_session_manager.get_nof_pdu_sessions(); }

  ue_index_t get_index() const { return index; };

private:
  ue_index_t               index;
  pdu_session_manager_impl pdu_session_manager;
};

} // namespace srs_cu_up
} // namespace srsran
