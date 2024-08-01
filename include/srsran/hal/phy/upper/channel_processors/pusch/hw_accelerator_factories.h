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

/// Configuration parameters of the bbdev-based PUSCH decoder hardware-accelerator factory.
struct bbdev_hwacc_pusch_dec_factory_configuration {
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;
  /// Defines if the soft-buffer is implemented in the accelerator or not.
  bool ext_softbuffer;
  /// Interfacing to an external HARQ buffer context repository.
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context;
  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue = true;
};

/// \brief Dynamic library w_accelerator_pdsch_enc_factory creation entry point.
///
/// Returns an instance of a PUSCH decoder hardware accelerator factory on success,
/// otherwise returns nullptr.
///
/// \param[in] accelerator_config Hardware-accelerator configuration.
/// \return Pointer to PUSCH decoding HW accelerator.
std::unique_ptr<hw_accelerator_pusch_dec_factory>
create_dynamic_bbdev_pusch_dec_acc_factory(const bbdev_hwacc_pusch_dec_factory_configuration& accelerator_config) asm(
    "create_dynamic_bbdev_pusch_dec_acc_factory");

/// \brief Creates a bbdev-based PUSCH decoder hardware-accelerator factory.
///
/// This function dynamically loads a shared library corresponding to the specified implementation name and retrieves
/// the factory creation function from it.
///
/// The naming convention for the shared library is as follows: if the implementation name is \c custom, the library
/// name is expected to be \c libsrsran_bbdev_pusch_dec_acc_custom.so. By default, the function tries to load
/// \c libsrsran_bbdev_pusch_dec_acc_srs.so. The dynamic library must contain the function prototype
/// \ref create_dynamic_bbdev_pusch_dec_acc_factory.
///
/// Ensure that \c LD_LIBRARY_PATH environment variable includes the path where the dynamic libraries are located.
///
/// \param[in] accelerator_config Hardware-accelerator configuration.
/// \param[in] impl_name          Selects the name of bbdev-based PUSCH decoder hardware-accelerator dynamic library for
/// example \e srs. \return A valid bbdev-based PUSCH decoder hardware-accelerator factory if the factory is
/// successfully created, otherwise \c nullptr.
std::unique_ptr<hw_accelerator_pusch_dec_factory>
create_bbdev_pusch_dec_acc_factory(const bbdev_hwacc_pusch_dec_factory_configuration& accelerator_config,
                                   std::string                                        impl_name = "srs");

} // namespace hal
} // namespace srsran
