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

#include "srsran/f1ap/cu_cp/du_setup_notifier.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Class responsible for handling the setup of the DUs that connect to this CU-CP.
class du_setup_handler
{
public:
  virtual ~du_setup_handler() = default;

  /// \brief Determines whether a DU setup should be accepted by the CU-CP, given the state of the CU-CP and resources
  /// available.
  virtual du_setup_result handle_du_setup_request(const du_setup_request& req) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran