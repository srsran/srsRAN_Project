/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"

namespace srsran {
namespace hal {

struct hw_accelerator_pdsch_enc_configuration {
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;
};

/// Returns an instance of a PDSCH encoder hardware accelerator factory on success,
/// otherwise returns nullptr.
/// \param[in] accelerator_config Hardware-accelerator configuration.
/// \return Pointer to PDSCH encoding HW accelerator.
std::shared_ptr<hw_accelerator_pdsch_enc_factory>
create_hw_accelerator_pdsch_enc_factory(const hw_accelerator_pdsch_enc_configuration& accelerator_config);

} // namespace hal
} // namespace srsran
