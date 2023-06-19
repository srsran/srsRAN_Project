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

#include "re_buffer.h"
#include "re_pattern.h"
#include "srsran/ran/precoding/precoding_configuration.h"

namespace srsran {

/// \brief Resource Element mapping interface.
///
/// Provides a generic interface that DL channel processors use to map Resource Elements into the resource grid.
class resource_grid_mapper
{
public:
  /// Default destructor.
  virtual ~resource_grid_mapper() = default;

  /// \brief Maps the input resource elements into the resource grid.
  /// \param[in] input      Input data.
  /// \param[in] pattern    Data allocation pattern in the resource grid.
  /// \param[in] precoding  Precoding configuration.
  virtual void
  map(const re_buffer_reader& input, const re_pattern_list& pattern, const precoding_configuration& precoding) = 0;

  /// \brief Maps the input resource elements into the resource grid.
  /// \param[in] input      Input data.
  /// \param[in] pattern    Data allocation pattern in the resource grid.
  /// \param[in] reserved   Reserved resource elements, to be excluded from the allocation pattern.
  /// \param[in] precoding  Precoding configuration.
  virtual void map(const re_buffer_reader&        input,
                   const re_pattern_list&         pattern,
                   const re_pattern_list&         reserved,
                   const precoding_configuration& precoding) = 0;
};

} // namespace srsran
