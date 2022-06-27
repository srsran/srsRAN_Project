/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_MAC_TEST_HELPERS_H
#define SRSGNB_MAC_TEST_HELPERS_H

#include "../../lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "srsgnb/du/du_cell_config_helpers.h"

namespace srsgnb {

namespace test_helpers {

/// Generates default MAC Cell Configuration to be used in unit tests.
inline mac_cell_creation_request make_default_mac_cell_config()
{
  du_cell_config            default_du_cell_cfg = du_config_helpers::make_default_du_cell_config();
  mac_cell_creation_request req                 = make_mac_cell_config(to_du_cell_index(0), default_du_cell_cfg);
  return req;
}

} // namespace test_helpers

} // namespace srsgnb

#endif // SRSGNB_MAC_TEST_HELPERS_H