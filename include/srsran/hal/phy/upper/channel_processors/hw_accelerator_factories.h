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

#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"

namespace srsran {
namespace hal {

/// Configuration parameters of the bbdev-based PDSCH encoder hardware-accelerator factory.
struct bbdev_hwacc_pdsch_enc_factory_configuration {
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;
  /// Defines if the PDSCH encoder operates in CB mode (true) or TB mode (false).
  bool cb_mode = false;
  /// Defines the maximum supported TB size in bytes (CB mode will be forced for larger TBs).
  /// Only used in TB mode.
  unsigned max_tb_size;
  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue;
};

/// \brief Creates a bbdev-based PDSCH encoder hardware accelerator factory.
/// \param[in] cfg    Configuration parameters of the bbdev-based PDSCH encoder hardware accelerator.
/// \return A valid PDSCH encoder hardware accelerator factory on success, otherwise \c nullptr.
std::shared_ptr<hw_accelerator_pdsch_enc_factory>
create_bbdev_pdsch_enc_acc_factory(const bbdev_hwacc_pdsch_enc_factory_configuration& accelerator_config);

} // namespace hal
} // namespace srsran
