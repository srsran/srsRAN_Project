
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

/// \file
/// \brief Channel equalization factories.

#pragma once

#include "srsran/phy/upper/equalization/channel_equalizer.h"

namespace srsran {

/// Describes a channel equalizer factory.
class channel_equalizer_factory
{
public:
  /// Default destructor.
  virtual ~channel_equalizer_factory() = default;

  /// \brief Creates a channel equalizer.
  /// \return A unique pointer to a channel equalizer instance.
  virtual std::unique_ptr<channel_equalizer> create() = 0;
};

/// Creates a Zero Forcing channel equalizer factory.
std::shared_ptr<channel_equalizer_factory> create_channel_equalizer_factory_zf();

} // namespace srsran
