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

#include "srsran/radio/radio_factory.h"

#ifdef ENABLE_UHD
#include "uhd/radio_uhd_impl.h"
#endif // ENABLE_UHD

#ifdef ENABLE_ZMQ
#include "zmq/radio_factory_zmq_impl.h"
#endif // ENABLE_ZMQ

#include "plugin_radio_factory.h"

using namespace srsran;

namespace {

struct radio_factory_entry {
  std::string                                     name;
  std::function<std::unique_ptr<radio_factory>()> make;
};

static const std::vector<radio_factory_entry> radio_factory_available_factories = {
#ifdef ENABLE_UHD
    {"uhd", []() { return std::make_unique<radio_factory_uhd_impl>(); }},
#endif // ENABLE_UHD
#ifdef ENABLE_ZMQ
    {"zmq", []() { return std::make_unique<radio_factory_zmq_impl>(); }},
#endif // ENABLE_ZMQ
};

} // namespace

void srsran::print_available_radio_factories()
{
  if (radio_factory_available_factories.empty()) {
    return;
  }

  // Print available factories.
  fmt::print("Available radio types: ");
  for (unsigned i = 0, e = radio_factory_available_factories.size(); i != e; ++i) {
    if (i > 0) {
      if (i == radio_factory_available_factories.size() - 1) {
        fmt::print(" and ");
      } else {
        fmt::print(", ");
      }
    }
    fmt::print("{}", radio_factory_available_factories[i].name);
  }
  fmt::print(".\n");
}

std::unique_ptr<radio_factory> srsran::create_radio_factory(std::string driver_name)
{
  // Convert driver name to lower case.
  for (char& c : driver_name) {
    c = std::tolower(c);
  }

  // Iterate all available driver names.
  for (const radio_factory_entry& entry : radio_factory_available_factories) {
    if (entry.name == driver_name) {
      return entry.make();
    }
  }

  // Try creating a plugin radio factory.
  auto factory = create_plugin_radio_factory(driver_name);
  if (factory) {
    return factory;
  }

  // No match, print available factories.
  fmt::print("Factory for radio type {} not found. Make sure to select a valid type.\n", driver_name);

  return nullptr;
}
