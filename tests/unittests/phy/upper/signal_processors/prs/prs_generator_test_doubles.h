/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"

namespace srsran {

/// Spy implementation of a PRS generator.
class prs_processor_spy : public prs_generator
{
  bool generate_method_called = false;

public:
  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override
  {
    generate_method_called = true;
  }

  /// Returns true if the map method has been called, false otherwise.
  bool is_generate_called() const { return generate_method_called; }
};

} // namespace srsran
