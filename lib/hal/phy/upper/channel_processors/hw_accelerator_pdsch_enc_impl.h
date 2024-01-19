/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Hardware accelerated PDSCH encoder functions declaration.

#pragma once

#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"

namespace srsran {
namespace hal {

/// Generic hardware accelerated PDSCH encoder functions.
class hw_accelerator_pdsch_enc_impl : public hw_accelerator_pdsch_enc
{
public:
  /// Default constructor.
  hw_accelerator_pdsch_enc_impl() = default;

  // See hw_accelerator interface for the documentation.
  bool enqueue_operation(span<const uint8_t> data, span<const uint8_t> aux_data = {}, unsigned cb_index = 0) override;
  // See hw_accelerator interface for the documentation.
  bool dequeue_operation(span<uint8_t> data, span<uint8_t> packed_data = {}, unsigned segment_index = 0) override;
  // See hw_accelerator interface for the documentation.
  void configure_operation(const hw_pdsch_encoder_configuration& config, unsigned cb_index = 0) override;

private:
  /// Hardware-specific implementation of the enqueue_operation function.
  virtual bool hw_enqueue(span<const uint8_t> data, unsigned cb_index) = 0;
  /// Hardware-specific implementation of the dequeue_operation function.
  virtual bool hw_dequeue(span<uint8_t> data, span<uint8_t> packed_data, unsigned segment_index) = 0;
  /// Hardware-specific configuration function.
  virtual void hw_config(const hw_pdsch_encoder_configuration& config, unsigned cb_index) = 0;
};

} // namespace hal
} // namespace srsran
