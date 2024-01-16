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

#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "dpdk.h"
#include <sstream>

using namespace srsran;
using namespace dpdk;

/// Splits the input string into a vector of substrings separated by space characters.
static std::vector<std::string> split_string_by_space(const std::string& input)
{
  std::vector<std::string> strings;

  std::istringstream ss(input);
  while (ss.good()) {
    std::string substr;
    std::getline(ss, substr, ' ');

    if (!substr.empty()) {
      strings.push_back(std::move(substr));
    }
  }

  return strings;
}

std::unique_ptr<dpdk_eal> srsran::dpdk::create_dpdk_eal(const std::string& args, srslog::basic_logger& logger)
{
  auto               strings = split_string_by_space(args);
  std::vector<char*> argv;
  for (const auto& s : strings) {
    argv.push_back(const_cast<char*>(s.c_str()));
  }

  // EAL initialization.
  if (!::eal_init(argv.size(), argv.data(), logger)) {
    return nullptr;
  }

  return std::make_unique<dpdk_eal>(logger);
}
