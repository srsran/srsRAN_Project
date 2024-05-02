/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_controller_factory.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<cu_cp_controller> srsran::srs_cu_cp::create_cu_cp_controller(cu_cp_routine_manager&    routine_mng,
                                                                             const ngap_configuration& ngap_cfg,
                                                                             ngap_connection_manager&  ngap_conn_mng,
                                                                             const cu_up_processor_repository& cu_ups)
{
  auto cu_cp_ctrl = std::make_unique<cu_cp_controller>(routine_mng, ngap_cfg, ngap_conn_mng, cu_ups);
  return cu_cp_ctrl;
}
