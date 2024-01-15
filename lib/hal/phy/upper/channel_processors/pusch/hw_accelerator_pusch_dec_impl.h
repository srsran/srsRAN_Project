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

/// \file
/// \brief Hardware accelerated PUSCH decoder functions declaration.

#pragma once

#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"

namespace srsran {
namespace hal {

/// Generic hardware accelerated PUSCH decoder function.
class hw_accelerator_pusch_dec_impl : public hw_accelerator_pusch_dec
{
public:
  /// Default constructor.
  hw_accelerator_pusch_dec_impl() = default;

  // See hw_accelerator interface for the documentation.
  bool enqueue_operation(span<const int8_t> data, span<const int8_t> soft_data = {}, unsigned cb_index = 0) override;
  // See hw_accelerator interface for the documentation.
  bool dequeue_operation(span<uint8_t> data, span<int8_t> soft_data = {}, unsigned segment_index = 0) override;
  // See hw_accelerator interface for the documentation.
  void configure_operation(const hw_pusch_decoder_configuration& config, unsigned cb_index = 0) override;
  // See hw_accelerator interface for the documentation.
  void
  read_operation_outputs(hw_pusch_decoder_outputs& out, unsigned cb_index = 0, unsigned absolute_cb_id = 0) override;
  // See hw_accelerator interface for the documentation.
  void free_harq_context_entry(unsigned absolute_cb_id) override;
  // See hw_accelerator interface for the documentation.
  bool is_external_harq_supported() const override;

private:
  /// Hardware-specific implementation of the enqueue_operation function.
  virtual bool hw_enqueue(span<const int8_t> data, span<const int8_t> soft_data, unsigned cb_index) = 0;
  /// Hardware-specific implementation of the dequeue_operation function.
  virtual bool hw_dequeue(span<uint8_t> data, span<int8_t> soft_data, unsigned segment_index) = 0;
  /// Hardware-specific configuration function.
  virtual void hw_config(const hw_pusch_decoder_configuration& config, unsigned cb_index) = 0;
  /// Hardware-specific operation status outputs recovery function.
  virtual void hw_read_outputs(hw_pusch_decoder_outputs& out, unsigned cb_index, unsigned absolute_cb_id) = 0;
  /// Hardware-specific HARQ buffer context freeing function.
  virtual void hw_free_harq_context(unsigned absolute_cb_id) = 0;
  /// Hardware-specific external HARQ buffer checking function.
  virtual bool is_hw_external_harq_supported() const = 0;
};

} // namespace hal
} // namespace srsran
