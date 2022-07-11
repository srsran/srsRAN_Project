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

#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

namespace srs_cu_cp {

struct rrc_ue_cfg_t {
  srslog::basic_logger&             logger = srslog::fetch_basic_logger("RRC");
};

} // namespace srs_cu_cp

} // namespace srsgnb
