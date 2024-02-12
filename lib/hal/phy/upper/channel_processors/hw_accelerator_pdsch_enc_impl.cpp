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

#include "hw_accelerator_pdsch_enc_impl.h"

using namespace srsran;
using namespace hal;

bool hw_accelerator_pdsch_enc_impl::enqueue_operation(span<const uint8_t> data,
                                                      span<const uint8_t> aux_data,
                                                      unsigned            cb_index)
{
  return hw_enqueue(data, cb_index);
}

bool hw_accelerator_pdsch_enc_impl::dequeue_operation(span<uint8_t> data,
                                                      span<uint8_t> packed_data,
                                                      unsigned      segment_index)
{
  return hw_dequeue(data, packed_data, segment_index);
}

void hw_accelerator_pdsch_enc_impl::configure_operation(const hw_pdsch_encoder_configuration& config, unsigned cb_index)
{
  hw_config(config, cb_index);
}
