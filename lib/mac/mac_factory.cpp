/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/mac/mac_factory.h"
#include "mac_impl.h"
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"

using namespace srsran;

std::unique_ptr<mac_interface> srsran::create_mac(const mac_config& mac_cfg)
{
  std::unique_ptr<mac_interface> macobj = std::make_unique<mac_impl>(mac_cfg);
  return macobj;
}
