/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_controller.h"
#include "../cu_up_processor/cu_up_processor_repository.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_controller::cu_cp_controller(cu_cp_routine_manager&            routine_manager_,
                                   ue_manager&                       ue_mng_,
                                   const ngap_configuration&         ngap_cfg_,
                                   ngap_connection_manager&          ngap_conn_mng_,
                                   const cu_up_processor_repository& cu_ups_,
                                   du_processor_repository&          dus,
                                   task_executor&                    ctrl_exec) :
  ue_mng(ue_mng_), cu_ups(cu_ups_), amf_mng(routine_manager_, ngap_cfg_, ngap_conn_mng_), du_mng(dus, ctrl_exec)
{
  (void)ue_mng;
}

bool cu_cp_controller::handle_du_setup_request(const du_setup_request& req)
{
  if (not amf_mng.is_amf_connected()) {
    // If AMF is not connected, it either means that the CU-CP is not operational state or there is a CU-CP failure.
    return false;
  }

  return true;
}

bool cu_cp_controller::request_ue_setup() const
{
  if (not amf_mng.is_amf_connected()) {
    return false;
  }

  if (cu_ups.get_nof_cu_ups() == 0) {
    return false;
  }

  return true;
}
