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
/// \brief Transform precoding factories.

#pragma once

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include <memory>

namespace srsran {

/// Factory that builds transform precoder objects.
class transform_precoder_factory
{
public:
  /// Default destructor.
  virtual ~transform_precoder_factory() = default;

  /// Creates and returns a transform precoder object.
  virtual std::unique_ptr<transform_precoder> create() = 0;
};

/// \brief Creates and returns a transform precoder factory based on the DFT algorithm.
///
/// \return A transform precoder factory.
std::shared_ptr<transform_precoder_factory>
create_dft_transform_precoder_factory(std::shared_ptr<dft_processor_factory>, unsigned max_nof_prb);

} // namespace srsran
