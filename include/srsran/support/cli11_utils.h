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

#include "external/CLI/CLI11.hpp"
#include "srsran/adt/optional.h"

namespace srsran {

/// \brief Parse string into optional type.
template <typename T>
bool lexical_cast(const std::string& in, srsran::optional<T>& output)
{
  using CLI::detail::lexical_cast;

  T val;
  if (not lexical_cast(in, val)) {
    return false;
  }
  output = val;
  return true;
}

/// \brief Parsing an integer with additional option "auto" into an optional of an enum type.
template <typename Param>
void add_auto_enum_option(CLI::App&          app,
                          const std::string& option_name,
                          optional<Param>&   param,
                          const std::string& desc)
{
  app.add_option_function<std::string>(
         option_name,
         [&param](const std::string& in) -> void {
           if (in.empty() or in == "auto") {
             return;
           }
           std::stringstream             ss(in);
           std::underlying_type_t<Param> val;
           ss >> val;
           param = (Param)val;
         },
         desc)
      ->check([](const std::string& in_str) -> std::string {
        if (in_str == "auto" or in_str.empty()) {
          return "";
        }
        // Check for a valid integer number;
        CLI::TypeValidator<int> IntegerValidator("INTEGER");
        return IntegerValidator(in_str);
      })
      ->default_str("auto");
}

} // namespace srsran