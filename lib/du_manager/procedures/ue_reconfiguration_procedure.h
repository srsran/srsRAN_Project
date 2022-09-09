/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_manager_config.h"
#include "../du_manager_interfaces.h"

namespace srsgnb {
namespace srs_du {

class ue_reconfiguration_procedure
{
public:
  ue_reconfiguration_procedure(const du_ue_config_update_request& request_, ue_manager_ctrl_configurer& ue_mng_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void add_bearers_in_ue_context();

  void update_mac_bearers();

  const du_ue_config_update_request request;
  ue_manager_ctrl_configurer&       ue_mng;
  srslog::basic_logger&             logger = srslog::fetch_basic_logger("DU-MNG");
  du_ue_context*                    ue     = nullptr;
};

} // namespace srs_du
} // namespace srsgnb