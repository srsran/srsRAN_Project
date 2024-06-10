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

#include "pusch_decoder_empty_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

pusch_decoder_empty_impl::pusch_decoder_empty_impl(unsigned nof_prb, unsigned nof_layers) :
  decoder_buffer(nof_prb, nof_layers)
{
}

pusch_decoder_buffer& pusch_decoder_empty_impl::new_data(span<uint8_t>                       transport_block,
                                                         unique_rx_buffer                    rm_buffer_,
                                                         pusch_decoder_notifier&             notifier_,
                                                         const pusch_decoder::configuration& cfg)
{
  decoder_buffer.new_data(transport_block, std::move(rm_buffer_), notifier_, cfg);
  return decoder_buffer;
}

void pusch_decoder_empty_impl::set_nof_softbits(units::bits /* nof_softbits */)
{
  // Do nothing.
}

void pusch_decoder_empty_impl::decoder_buffer_impl::new_data(span<uint8_t>                       transport_block,
                                                             srsran::unique_rx_buffer            rm_buffer_,
                                                             srsran::pusch_decoder_notifier&     notifier_,
                                                             const pusch_decoder::configuration& cfg)
{
  units::bytes tb_size = units::bytes(transport_block.size());

  // Erase transport block contents from a previous transmission.
  srsvec::zero(transport_block);

  // Save inputs.
  rm_buffer = std::move(rm_buffer_);
  srsran_assert(rm_buffer.is_valid(), "Invalid buffer.");
  notifier       = &notifier_;
  is_new_data    = cfg.new_data;
  softbits_count = 0;
  nof_codeblocks = ldpc::compute_nof_codeblocks(tb_size.to_bits(), cfg.base_graph);

  // Calculate full code block size.
  unsigned lifting_size = ldpc::compute_lifting_size(tb_size.to_bits(), cfg.base_graph, nof_codeblocks);
  codeblock_size        = ldpc::compute_codeblock_size(cfg.base_graph, lifting_size);
}

span<log_likelihood_ratio> pusch_decoder_empty_impl::decoder_buffer_impl::get_next_block_view(unsigned block_size)
{
  // Return always the first softbits, the decoder will ignore the soft bits.
  return span<log_likelihood_ratio>(softbits_buffer).first(block_size);
}

void pusch_decoder_empty_impl::decoder_buffer_impl::on_new_softbits(span<const log_likelihood_ratio> softbits)
{
  // Accumulate the count of softbits.
  softbits_count += softbits.size();
}

void pusch_decoder_empty_impl::decoder_buffer_impl::on_end_softbits()
{
  // Make sure the notifier is valid.
  srsran_assert(notifier != nullptr, "Invalid notifier.");

  // Prepare failed transport block.
  pusch_decoder_result result;
  result.ldpc_decoder_stats.reset();
  result.nof_codeblocks_total = 0;
  result.tb_crc_ok            = false;

  // Reset rate matching data if it a new transmission.
  if (is_new_data) {
    for (unsigned i_cb = 0; i_cb != nof_codeblocks; ++i_cb) {
      srsvec::zero(rm_buffer->get_codeblock_soft_bits(i_cb, codeblock_size.value()));
    }
  }

  // Release buffer.
  rm_buffer.release();

  // Notify completion.
  notifier->on_sch_data(result);
}
