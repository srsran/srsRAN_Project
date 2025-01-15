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
