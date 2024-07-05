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

using cli11_cell = std::vector<std::string>;

/// \brief Adds a subcommand to the given application using the given subcommand name and description.
///
/// If the subcommand already exists in the application, returns a pointer to it.
///
/// \param app Application where the subcommand will be added.
/// \param name Subcommand name.
/// \param desc Human readable description of the subcommand.
/// \return A pointer to the subcommand added to the application.
inline CLI::App* add_subcommand(CLI::App& app, const std::string& name, const std::string& desc)
{
  try {
    if (CLI::App* subcommand = app.get_subcommand(name)) {
      return subcommand;
    }
  } catch (CLI::OptionNotFound& e) {
  }

  return app.add_subcommand(name, desc)->configurable();
}

/// \brief Adds an option to the given application.
///
/// This function adds an option to the given application using the given parameters. If the option is already present
/// in the application, it is removed and a new option is added that will call the callback of the deleted callback
/// and the conversion of the result for the given parameter. By doing this, it allows to add multiple parameters for
/// one option, so one option will be present in the configuration but the result will be written in all the
/// parameters registered for that option.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param param Parameter where the option value will be stored after parsing.
/// \param desc Human readable description of the option.
/// \return A pointer to the option added to the application.
template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& option_name, T& param, const std::string& desc)
{
  auto* opt = app.get_option_no_throw(option_name);
  if (!opt) {
    return app.add_option(option_name, param, desc);
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option(
          option_name,
          [&param, callback = std::move(callbck)](const CLI::results_t& res) {
            callback(res);
            return CLI::detail::lexical_conversion<T, T>(res, param);
          },
          desc,
          false,
          [&param]() -> std::string { return CLI::detail::checked_to_string<T, T>(param); })
      ->run_callback_for_default();
}

/// Specialization for bools than changes the default function for capture the default string.
template <>
inline CLI::Option* add_option(CLI::App& app, const std::string& option_name, bool& param, const std::string& desc)
{
  auto* opt = app.get_option_no_throw(option_name);
  if (!opt) {
    return app.add_option(option_name, param, desc)->default_function([&param]() -> std::string {
      return param ? "true" : "false";
    });
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option(
          option_name,
          [&param, callback = std::move(callbck)](const CLI::results_t& res) {
            callback(res);
            return CLI::detail::lexical_conversion<bool, bool>(res, param);
          },
          desc,
          false,
          [&param]() -> std::string { return param ? "true" : "false"; })
      ->run_callback_for_default();
}

/// \brief Adds an option function to the given application.
///
/// This function adds an option function to the given application using the given parameters. If the option is
/// already present in the application, it is removed and a new option is added that will contain the given function
/// and deleted callback as function. By doing this, it allows to add multiple parameters for one option, so one
/// option will be present in the configuration and the all the functions registered for that option will be called.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param func Function to execute during parsing.
/// \param desc Human readable description of the option.
/// \return A pointer to the option added to the application.
template <typename T>
CLI::Option* add_option_function(CLI::App&                            app,
                                 const std::string&                   option_name,
                                 const std::function<void(const T&)>& func,
                                 const std::string&                   desc)
{
  auto* opt = app.get_option_no_throw(option_name);
  if (!opt) {
    return app.add_option_function<T>(option_name, func, desc)->run_callback_for_default();
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option_function<T>(
          option_name,
          [func, callback = std::move(callbck)](const std::string& value) {
            func(value);
            callback({value});
          },
          desc)
      ->run_callback_for_default();
}

/// \brief Adds an option of type cell to the given application.
///
/// \param app Application where the option will be added.
/// \param option_name Option name.
/// \param func Function to execute during parsing.
/// \param desc Human readable description of the option.
/// \return A pointer to the option added to the application.
inline CLI::Option* add_option_cell(CLI::App&                                     app,
                                    const std::string&                            option_name,
                                    const std::function<void(const cli11_cell&)>& func,
                                    const std::string&                            desc)
{
  auto* opt = app.get_option_no_throw(option_name);
  if (!opt) {
    return app.add_option_function<std::vector<std::string>>(option_name, func, desc);
  }

  // Option was found. Get the callback and create new option.
  auto callbck = opt->get_callback();
  app.remove_option(opt);

  return app
      .add_option_function<cli11_cell>(
          option_name,
          [func, callback = std::move(callbck)](const cli11_cell& value) {
            func(value);
            callback(value);
          },
          desc)
      ->run_callback_for_default();
}

/// Parse string into optional type.
template <typename T>
bool lexical_cast(const std::string& in, std::optional<T>& output)
{
  using CLI::detail::lexical_cast;

  T val;
  if (not lexical_cast(in, val)) {
    return false;
  }
  output = val;
  return true;
}

/// Parsing an integer with additional option "auto" into an optional of an enum type.
template <typename Param>
void add_auto_enum_option(CLI::App&             app,
                          const std::string&    option_name,
                          std::optional<Param>& param,
                          const std::string&    desc)
{
  add_option_function<std::string>(
      app,
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
