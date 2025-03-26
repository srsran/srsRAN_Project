/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/mac/cell_configuration.h"
#include "srsran/support/units.h"

namespace srsran {

using si_version_type       = unsigned;
using si_message_index_type = unsigned;

/// SI message update outcome.
struct si_change_result {
  /// Current SI version.
  si_version_type version = 0;
  /// Length of SIB1.
  units::bytes sib1_len{0};
  /// SI message lengths.
  std::vector<units::bytes> si_msg_lens;
};

/// Interface used to update the content of SI messages being broadcast.
class cell_sys_info_configurator
{
public:
  virtual ~cell_sys_info_configurator() = default;

  /// Handle a request to change broadcast SI messages.
  virtual si_change_result handle_si_change_request(const mac_cell_sys_info_config& request) = 0;
};

} // namespace srsran
