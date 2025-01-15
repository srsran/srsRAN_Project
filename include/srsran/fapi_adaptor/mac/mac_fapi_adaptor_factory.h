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

#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor_config.h"

namespace srsran {
namespace fapi_adaptor {

/// Factory to create \c mac_fapi_adaptor objects.
class mac_fapi_adaptor_factory
{
public:
  virtual ~mac_fapi_adaptor_factory() = default;

  /// Creates a \c mac_fapi_adaptor object using the given configuration and dependencies.
  virtual std::unique_ptr<mac_fapi_adaptor> create(const mac_fapi_adaptor_config&  config,
                                                   mac_fapi_adaptor_dependencies&& dependencies) = 0;
};

/// Creates a \c mac_fapi_adaptor_factory object.
std::unique_ptr<mac_fapi_adaptor_factory> create_mac_fapi_adaptor_factory();

} // namespace fapi_adaptor
} // namespace srsran
