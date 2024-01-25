/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "amf_connection_manager.h"
#include "node_connection_notifier.h"
#include "srsran/cu_cp/cu_cp_configuration.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Class responsible for tracking and managing the state of the connections between the CU-CP and other nodes,
/// namely DUs, CU-UPs and AMF.
class node_connection_manager
{
public:
  node_connection_manager(cu_cp_routine_manager&          routine_manager_,
                          const ngap_configuration&       ngap_cfg_,
                          ngap_cu_cp_connection_notifier& cu_cp_amf_conn_notif_,
                          cu_cp_ngap_control_notifier&    ngap_ctrl_notif_);

  amf_connection_manager& amf_connection_handler() { return amf_mng; }

  bool handle_du_setup_request(const du_setup_request& req);

private:
  amf_connection_manager amf_mng;
};

} // namespace srs_cu_cp
} // namespace srsran