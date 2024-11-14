
/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Precoding factories.

#pragma once

#include "srsran/phy/generic_functions/precoding/channel_precoder.h"

namespace srsran {

/// Factory that builds channel precoder objects.
class channel_precoder_factory
{
public:
  /// Default destructor.
  virtual ~channel_precoder_factory() = default;

  /// Creates and returns a channel precoder object.
  virtual std::unique_ptr<channel_precoder> create() = 0;
};

/// \brief Creates and returns a channel precoder factory.
///
/// \param[in] precoder_type A string indicating the precoder implementation to create. Values: generic, auto, avx2.
/// \return A channel precoder factory for the chosen precoder implementation.
std::shared_ptr<channel_precoder_factory> create_channel_precoder_factory(const std::string& precoder_type);

} // namespace srsran
