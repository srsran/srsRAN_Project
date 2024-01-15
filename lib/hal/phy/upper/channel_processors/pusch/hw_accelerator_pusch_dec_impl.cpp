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

#include "hw_accelerator_pusch_dec_impl.h"

using namespace srsran;
using namespace hal;

bool hw_accelerator_pusch_dec_impl::enqueue_operation(span<const int8_t> data,
                                                      span<const int8_t> soft_data,
                                                      unsigned           cb_index)
{
  return hw_enqueue(data, soft_data, cb_index);
}

bool hw_accelerator_pusch_dec_impl::dequeue_operation(span<uint8_t> data,
                                                      span<int8_t>  soft_data,
                                                      unsigned      segment_index)
{
  return hw_dequeue(data, soft_data, segment_index);
}

void hw_accelerator_pusch_dec_impl::configure_operation(const hw_pusch_decoder_configuration& config, unsigned cb_index)
{
  hw_config(config, cb_index);
}

void hw_accelerator_pusch_dec_impl::read_operation_outputs(hw_pusch_decoder_outputs& out,
                                                           unsigned                  cb_index,
                                                           unsigned                  absolute_cb_id)
{
  hw_read_outputs(out, cb_index, absolute_cb_id);
}

void hw_accelerator_pusch_dec_impl::free_harq_context_entry(unsigned absolute_cb_id)
{
  hw_free_harq_context(absolute_cb_id);
}

bool hw_accelerator_pusch_dec_impl::is_external_harq_supported() const
{
  return is_hw_external_harq_supported();
}
