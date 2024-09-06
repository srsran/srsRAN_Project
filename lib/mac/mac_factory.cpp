/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/mac/mac_factory.h"
#include "mac_impl.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"

using namespace srsran;

std::unique_ptr<mac_interface> srsran::create_mac(const mac_config& mac_cfg)
{
  std::unique_ptr<mac_interface> mac_obj = std::make_unique<mac_impl>(mac_cfg);
  return mac_obj;
}
