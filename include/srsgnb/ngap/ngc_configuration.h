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

namespace srsgnb {

namespace srs_cu_cp {

/// \brief NGC configuration
struct ngc_configuration {
  unsigned    gnb_id = 0;
  std::string ran_node_name;
  std::string plmn; /// Full PLMN as string (without possible filler digit) e.g. "00101"
  unsigned    tac;
};

} // namespace srs_cu_cp

} // namespace srsgnb