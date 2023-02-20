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