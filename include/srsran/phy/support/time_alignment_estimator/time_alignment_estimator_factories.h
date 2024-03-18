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

#pragma once

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include <memory>

namespace srsran {

/// Time alignment estimator factory interface.
class time_alignment_estimator_factory
{
public:
  /// Default destructor.
  virtual ~time_alignment_estimator_factory() = default;

  /// Creates a time alignment estimator.
  virtual std::unique_ptr<time_alignment_estimator> create() = 0;
};

/// Creates a time alignment estimator factory based on DFT.
std::shared_ptr<time_alignment_estimator_factory>
create_time_alignment_estimator_dft_factory(std::shared_ptr<dft_processor_factory> dft_proc_factory);

} // namespace srsran
