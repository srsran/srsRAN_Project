/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "node_connection_manager.h"

using namespace srsran;
using namespace srs_cu_cp;

node_connection_manager::node_connection_manager(cu_cp_routine_manager&          routine_manager_,
                                                 const ngap_configuration&       ngap_cfg_,
                                                 ngap_cu_cp_connection_notifier& cu_cp_amf_conn_notif_,
                                                 cu_cp_ngap_control_notifier&    ngap_ctrl_notif_) :
  amf_mng(routine_manager_, ngap_cfg_, cu_cp_amf_conn_notif_, ngap_ctrl_notif_)
{
}

bool node_connection_manager::handle_du_setup_request(const du_setup_request& req)
{
  if (not amf_mng.is_amf_connected()) {
    // If AMF is not connected, it either means that the CU-CP is not operational state or there is a CU-CP failure.
    return false;
  }

  // TODO: Check if there is at least one CU-UP connected to the CU-CP.

  return true;
}
