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

#include "apps/services/buffer_pool/buffer_pool_appconfig.h"

namespace srsran {
namespace app_services {

/// Buffer pool manager for the applications.
class buffer_pool_manager
{
public:
  explicit buffer_pool_manager(const buffer_pool_appconfig& config)
  {
    init_byte_buffer_segment_pool(config.nof_segments, config.segment_size);
  }
};

} // namespace app_services
} // namespace srsran
