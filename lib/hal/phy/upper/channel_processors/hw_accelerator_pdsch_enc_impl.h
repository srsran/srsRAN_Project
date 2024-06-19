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
  void reserve_queue() override;
  // See hw_accelerator interface for the documentation.
  void free_queue() override;
  // See hw_accelerator interface for the documentation.
  bool enqueue_operation(span<const uint8_t> data, span<const uint8_t> aux_data = {}, unsigned cb_index = 0) override;
  // See hw_accelerator interface for the documentation.
  bool dequeue_operation(span<uint8_t> data, span<uint8_t> packed_data = {}, unsigned segment_index = 0) override;
  // See hw_accelerator interface for the documentation.
  void configure_operation(const hw_pdsch_encoder_configuration& config, unsigned cb_index = 0) override;
  // See hw_accelerator interface for the documentation.
  bool get_cb_mode() const override;
  // See hw_accelerator interface for the documentation.
  unsigned get_max_tb_size() const override;

private:
  /// Hardware-specific implementation of the reserve queue function.
  virtual void hw_reserve_queue() = 0;
  /// Hardware-specific implementation of the free queue function.
  virtual void hw_free_queue() = 0;
  /// Hardware-specific implementation of the enqueue_operation function.
  virtual bool hw_enqueue(span<const uint8_t> data, unsigned cb_index) = 0;
  /// Hardware-specific implementation of the dequeue_operation function.
  virtual bool hw_dequeue(span<uint8_t> data, span<uint8_t> packed_data, unsigned segment_index) = 0;
  /// Hardware-specific configuration function.
  virtual void hw_config(const hw_pdsch_encoder_configuration& config, unsigned cb_index) = 0;
  /// Hardware-specific CB mode quering function.
  virtual bool get_hw_cb_mode() const = 0;
  /// Hardware-specific maximum supported TB size quering function.
  virtual unsigned get_hw_max_tb_size() const = 0;
};

} // namespace hal
} // namespace srsran
