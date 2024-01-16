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

#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"

using namespace srsran;
using namespace hal;

namespace srsran {
namespace hal {

struct hw_accelerator_pusch_dec_configuration {
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;
  /// Defines if the soft-buffer is implemented in the accelerator or not.
  bool ext_softbuffer;
  /// Interfacing to an external HARQ buffer context repository.
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context;
};

// Returns an instance of a PUSCH decoder hardware accelerator factory on success,
/// otherwise returns nullptr.
/// \param[in] accelerator_config Hardware-accelerator configuration.
/// \return Pointer to PUSCH decoding HW accelerator.
std::shared_ptr<hw_accelerator_pusch_dec_factory>
create_hw_accelerator_pusch_dec_factory(const hw_accelerator_pusch_dec_configuration& accelerator_config);

} // namespace hal
} // namespace srsran
