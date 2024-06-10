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
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"

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

/// Returns an instance of a PUSCH decoder hardware accelerator factory on success,
/// otherwise returns nullptr.
/// \param[in] accelerator_config Hardware-accelerator configuration.
/// \return Pointer to PUSCH decoding HW accelerator.
std::shared_ptr<hw_accelerator_pusch_dec_factory>
create_hw_accelerator_pusch_dec_factory(const hw_accelerator_pusch_dec_configuration& accelerator_config);

} // namespace hal
} // namespace srsran
