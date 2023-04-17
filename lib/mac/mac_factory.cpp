/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/mac/mac_factory.h"
#include "mac_impl.h"
#include "mac_test_mode_impl.h"
#include "srsran/du_high/du_high_cell_executor_mapper.h"

using namespace srsran;

std::unique_ptr<mac_interface> srsran::create_mac(const mac_config& mac_cfg)
{
  if (mac_cfg.test_ue.has_value()) {
    return std::make_unique<mac_test_mode_impl>(mac_cfg);
  }
  return std::make_unique<mac_impl>(mac_cfg);
}
