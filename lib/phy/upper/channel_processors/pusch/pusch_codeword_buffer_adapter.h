/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"

namespace srsran {

/// Adapts a PUSCH codeword buffer interface to a PUSCH decoder interface.
class pusch_codeword_buffer_adapter : public pusch_codeword_buffer
{
public:
  void connect(pusch_decoder_buffer& decoder_buffer_) { decoder_buffer = &decoder_buffer_; }

  // See interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    srsran_assert(decoder_buffer != nullptr, "Decoder not connected.");
    return decoder_buffer->get_next_block_view(block_size);
  }

  // See interface for documentation.
  void on_new_block(span<const log_likelihood_ratio> demodulated, span<const log_likelihood_ratio> descrambled) override
  {
    srsran_assert(decoder_buffer != nullptr, "Decoder not connected.");
    decoder_buffer->on_new_softbits(descrambled);
  }

  // See interface for documentation.
  void on_end_codeword() override
  {
    srsran_assert(decoder_buffer != nullptr, "Decoder not connected.");
    decoder_buffer->on_end_softbits();
  }

private:
  pusch_decoder_buffer* decoder_buffer;
};

} // namespace srsran