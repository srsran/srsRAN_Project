/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
