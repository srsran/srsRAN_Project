/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include <string>

namespace srsran {
namespace dpdk {

/// Describes a bbdev accelerator factory for a determined accelerator implementation.
class bbdev_acc_factory
{
public:
  /// Default destructor.
  virtual ~bbdev_acc_factory() = default;

  /// \brief Creates a bbdev accelerator instance with the given configuration parameters.
  /// \param[in] cfg    Configuration of the bbdev hardware-accelerator.
  /// \param[in] logger SRS logger.
  /// \return The ownership to a bbdev accelerator on success, otherwise returns nullptr.
  virtual std::shared_ptr<bbdev_acc> create(const bbdev_acc_configuration& cfg, srslog::basic_logger& logger) = 0;
};

/// \brief Dynamic library bbdev accelerator factory creation entry point.
///
/// The dynamic library handler loads this function from an external library to create a bbdev accelerator factory.
std::unique_ptr<bbdev_acc_factory> create_dynamic_bbdev_acc_factory() asm("create_dynamic_bbdev_acc_factory");

/// \brief Creates a bbdev accelerator factory.
///
/// This function dynamically loads a shared library corresponding to the specified implementation name and retrieves
/// the factory creation function from it.
///
/// The naming convention for the shared library is as follows: if the implementation name is \c custom, the library
/// name is expected to be \c libsrsran_bbdev_acc_custom.so. By default, the function tries to load
/// \c libsrsran_bbdev_acc_srs.so. The dynamic library must contain the function prototype
/// \ref create_dynamic_bbdev_acc_factory.
///
/// Ensure that \c LD_LIBRARY_PATH environment variable includes the path where the dynamic libraries are located.
///
/// \param[in] impl_name Selects the name of bbdev accelerator dynamic library for example \e srs.
/// \return A valid bbdev accelerator factory if the factory is successfully created, otherwise \c nullptr.
std::unique_ptr<bbdev_acc_factory> create_bbdev_acc_factory(std::string impl_name = "srs");

} // namespace dpdk
} // namespace srsran
