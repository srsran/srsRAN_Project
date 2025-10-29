/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "mac_fapi_fastpath_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_fastpath_adaptor_config.h"

namespace srsran {
namespace fapi_adaptor {

/// Factory to create MAC-FAPI fastpath adaptors.
class mac_fapi_fastpath_adaptor_factory
{
public:
  virtual ~mac_fapi_fastpath_adaptor_factory() = default;

  /// Creates a MAC-FAPI fastpath adaptor using the given configuration and dependencies.
  virtual std::unique_ptr<mac_fapi_fastpath_adaptor> create(const mac_fapi_fastpath_adaptor_config& config,
                                                            mac_fapi_fastpath_adaptor_dependencies  dependencies) = 0;
};

/// Creates a MAC-FAPI fastpath adaptor factory.
std::unique_ptr<mac_fapi_fastpath_adaptor_factory> create_mac_fapi_fastpath_adaptor_factory();

} // namespace fapi_adaptor
} // namespace srsran
