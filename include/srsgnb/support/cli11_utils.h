/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "external/CLI/CLI11.hpp"
#include "srsgnb/adt/optional.h"

namespace srsgnb {

/// \brief Parse string into optional type.
template <typename T>
bool lexical_cast(const std::string& in, srsgnb::optional<T>& output)
{
  using CLI::detail::lexical_cast;

  T val;
  if (not lexical_cast(in, val)) {
    return false;
  }
  output = val;
  return true;
}

} // namespace srsgnb