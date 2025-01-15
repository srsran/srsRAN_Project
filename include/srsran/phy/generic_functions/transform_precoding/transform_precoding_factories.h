/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
