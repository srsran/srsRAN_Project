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

#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_phy_capabilities.h"
#include "srsran/support/compiler.h"

using namespace srsran;

#ifndef PUSCH_PROCESSOR_MAX_NOF_LAYERS
#define PUSCH_PROCESSOR_MAX_NOF_LAYERS 1
#endif // PUSCH_PROCESSOR_MAX_NOF_LAYERS

pusch_processor_phy_capabilities srsran::get_pusch_processor_phy_capabilities()
{
  return {.max_nof_layers = PUSCH_PROCESSOR_MAX_NOF_LAYERS};
}
