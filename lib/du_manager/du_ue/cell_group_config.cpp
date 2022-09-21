/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_group_config.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

cell_group_config srsgnb::srs_du::make_initial_ue_cell_group_config()
{
  cell_group_config cell_group;

  return cell_group;
}

cell_group_config srsgnb::srs_du::make_reconf_ue_cell_group_config(const f1ap_ue_config_update_request& request)
{
  cell_group_config cell_group;

  return cell_group;
}
