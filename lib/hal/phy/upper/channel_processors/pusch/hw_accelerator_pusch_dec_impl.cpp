/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "hw_accelerator_pusch_dec_impl.h"

using namespace srsran;
using namespace hal;

void hw_accelerator_pusch_dec_impl::reserve_queue()
{
  hw_reserve_queue();
}

void hw_accelerator_pusch_dec_impl::free_queue()
{
  hw_free_queue();
}

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
