
/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// \file
/// \brief Precoding factories.

#pragma once

#include "channel_precoder.h"

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
