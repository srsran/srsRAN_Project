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

#include "srsgnb/adt/optional.h"

namespace srsgnb {

struct crit_diagnostics_item {
  std::string iecrit;
  uint32_t    ie_id;
  std::string type_of_error;
};

struct crit_diagnostics_t {
  std::vector<crit_diagnostics_item> ies_crit_diagnostics = {};
  optional<uint16_t>                 proc_code;
  optional<std::string>              trigger_msg;
  optional<std::string>              proc_crit;
  optional<uint16_t>                 transaction_id;
};

} // namespace srsgnb
