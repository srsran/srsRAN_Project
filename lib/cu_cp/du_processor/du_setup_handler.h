/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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