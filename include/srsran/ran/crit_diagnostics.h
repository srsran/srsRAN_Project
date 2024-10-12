/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

struct crit_diagnostics_item {
  std::string iecrit;
  uint32_t    ie_id;
  std::string type_of_error;
};

struct crit_diagnostics_t {
  std::vector<crit_diagnostics_item> ies_crit_diagnostics = {};
  std::optional<uint16_t>            proc_code;
  std::optional<std::string>         trigger_msg;
  std::optional<std::string>         proc_crit;
  std::optional<uint16_t>            transaction_id;
};

} // namespace srsran
