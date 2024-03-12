/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
