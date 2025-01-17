/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/expected.h"
#include <string>

namespace srsran {

struct prs_generator_configuration;

/// \brief PRS generator validator.
class prs_generator_validator
{
public:
  /// Default destructor.
  virtual ~prs_generator_validator() = default;

  /// \brief Validates PRS generator configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const prs_generator_configuration& config) const = 0;
};

} // namespace srsran
