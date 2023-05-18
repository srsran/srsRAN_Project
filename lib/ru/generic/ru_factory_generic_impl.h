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

#include "srsran/ru/ru_factory.h"

namespace srsran {

/// \brief Radio Unit factory generic implementation.
///
/// Creates and returns Radio Units of type generic.
class ru_factory_generic_impl : public ru_factory
{
public:
  // See interface for documentation.
  std::unique_ptr<radio_unit> create(ru_config& config) override;
};

} // namespace srsran
