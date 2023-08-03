/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/srslog/logger.h"

namespace srsran {
namespace dpdk {

/// Interfacing to DPDK's EAL.
class dpdk_eal
{
public:
  /// Constructor.
  /// \param[in] logger SRS logger.
  explicit dpdk_eal(srslog::basic_logger& logger_);

  /// Default destructor.
  ~dpdk_eal();

  // Returns the internal SRS logger.
  /// \return SRS logger.
  srslog::basic_logger& get_logger() { return logger; }

private:
  /// SRS logger.
  srslog::basic_logger& logger;
};

} // namespace dpdk
} // namespace srsran
