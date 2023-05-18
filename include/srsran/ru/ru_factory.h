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

#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

class ru_config;

/// Abstract Radio Unit factory.
class ru_factory
{
public:
  /// Default destructor.
  virtual ~ru_factory() = default;

  /// Creates and returns a Radio Unit.
  virtual std::unique_ptr<radio_unit> create(ru_config& config) = 0;
};

/// Returns a Radio Unit factory.
std::unique_ptr<ru_factory> create_ru_factory();

} // namespace srsran
