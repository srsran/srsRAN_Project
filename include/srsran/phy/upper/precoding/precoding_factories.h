
/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Precoding factories.

#pragma once

#include "srsran/phy/upper/precoding/channel_precoder.h"

namespace srsran {
/// Factory that builds channel precoder objects.
class channel_precoder_factory
{
public:
  virtual ~channel_precoder_factory() = default;

  /// Creates and returns a channel precoder object.
  virtual std::unique_ptr<channel_precoder> create() = 0;
};

/// Creates and returns a channel precoder factory.
std::shared_ptr<channel_precoder_factory> create_channel_precoder_factory();
} // namespace srsran
