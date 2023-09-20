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

#pragma once

#include "resource_grid_test_doubles.h"

namespace srsran {

/// Resource grid mapper dummy.
class resource_grid_mapper_dummy : public resource_grid_mapper
{
public:
  void map(const re_buffer_reader&        input,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding) override
  {
  }

  void
  map(const re_buffer_reader& input, const re_pattern_list& pattern, const precoding_configuration& precoding) override
  {
  }

  void map(symbol_buffer&                 buffer,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding) override
  {
  }
};

} // namespace srsran
