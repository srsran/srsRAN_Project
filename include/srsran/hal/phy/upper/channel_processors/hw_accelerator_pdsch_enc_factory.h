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

#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"

namespace srsran {
namespace hal {

/// PDSCH encoder hardware accelerator factory.
class hw_accelerator_pdsch_enc_factory
{
public:
  virtual ~hw_accelerator_pdsch_enc_factory()                = default;
  virtual std::unique_ptr<hw_accelerator_pdsch_enc> create() = 0;
};

} // namespace hal
} // namespace srsran
