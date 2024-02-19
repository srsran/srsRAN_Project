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

#include "srsran/radio/radio_configuration.h"
#include "srsran/radio/radio_session.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {

/// Describes a radio session factory for a determined radio implementation.
class radio_factory : public radio_base
{
public:
  /// Returns the reference to the radio configuration validator for the given factory.
  virtual const radio_configuration::validator& get_configuration_validator() = 0;

  /// \brief Creates a new radio session with the given configuration parameters.
  /// \param[in] config Provides the configuration parameters.
  /// \param[in] task_executor Provides a task executor for executing asynchronous tasks.
  /// \param[in] notifier Provides radio event notifier interface.
  /// \return The ownership to a radio session if the session was successfully created.
  virtual std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                                task_executor&                    async_task_executor,
                                                radio_notification_handler&       notifier) = 0;
};

/// \brief Dynamic library radio factory creation entry point.
///
/// Implement a radio factory with this function for loading the radio factory through a dynamic library.
std::unique_ptr<radio_factory> create_dynamic_radio_factory();

/// \brief Creates a radio factory by selecting an existent factory or loading the appropriate dynamic library.
///
/// By default, this function creates a radio factory from the statically linked ones (i.e., \c uhd).
///
/// If the driver_name does not match with one of the statically linked factories, this function dynamically loads a
/// shared library corresponding to the specified driver name and retrieves the factory creation function from it.
///
/// The naming convention for the shared library is as follows: if the driver name is \c custom, the library name is
/// expected to be \c libsrsran_radio_custom.so. The factory creation function within the dynamic library is expected to
/// implement the function prototype \ref create_dynamic_radio_factory.
///
/// Ensure that the LD_LIBRARY_PATH environment variable includes the path where the dynamic libraries are located.
///
/// \param[in] driver_name Selects the type of radio factory for example \e uhd or \e zmq.
/// \return A valid radio factory if the factory is successfully created, otherwise \c nullptr.
std::unique_ptr<radio_factory> create_radio_factory(std::string driver_name);

/// \brief Helper function to print all statically linked available radio factories to \c stdout.
void print_available_radio_factories();

} // namespace srsran
