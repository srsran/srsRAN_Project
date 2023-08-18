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
#include "pusch_uci_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/channel_processors/uci_decoder.h"
#include "srsran/ran/uci/uci_constants.h"

namespace srsran {

class pusch_uci_decoder_wrapper : private pusch_decoder_buffer
{
public:
  pusch_uci_decoder_wrapper(uci_decoder& decoder_, unsigned max_nof_softbits) :
    decoder(decoder_), softbits_buffer(max_nof_softbits)
  {
  }

  pusch_decoder_buffer&
  new_transmission(unsigned message_length, modulation_scheme modulation, pusch_uci_decoder_notifier& notifier_)
  {
    notifier                  = &notifier_;
    softbits_count            = 0;
    message                   = span<uint8_t>(temp_message).first(message_length);
    decoder_config.modulation = modulation;
    return *this;
  }

private:
  /// UCI decoder.
  uci_decoder& decoder;
  /// Decode Notifier.
  pusch_uci_decoder_notifier* notifier = nullptr;
  /// Codeword softbits buffer.
  std::vector<log_likelihood_ratio> softbits_buffer;
  /// Current number of softbits in the buffer.
  unsigned softbits_count;
  /// Decoded message storage.
  std::array<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> temp_message;
  /// Current decoded message.
  span<uint8_t> message;
  /// Current UCI decoder configuration.
  uci_decoder::configuration decoder_config;

  // See interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    // Makes sure the block size does not overflow the buffer.
    srsran_assert(softbits_count + block_size <= softbits_buffer.size(),
                  "The sum of current buffer number of elements (i.e., {}) and the block size (i.e., {}), exceeds the "
                  "total number of elements of the buffer (i.e., {}).",
                  softbits_count,
                  block_size,
                  softbits_buffer.size());

    return span<log_likelihood_ratio>(softbits_buffer).subspan(softbits_count, block_size);
  }

  // See interface for documentation.
  void on_new_softbits(span<const log_likelihood_ratio> softbits) override
  {
    span<log_likelihood_ratio> block = get_next_block_view(softbits.size());

    // Copy only if the soft bits do not match.
    if (block.data() != softbits.data()) {
      srsvec::copy(block, softbits);
    }

    softbits_count += softbits.size();
  }

  // See interface for documentation.
  void on_end_softbits() override
  {
    // Select codeword from the soft bit buffer.
    span<const log_likelihood_ratio> codeword = span<const log_likelihood_ratio>(softbits_buffer).first(softbits_count);

    // Actual decode.
    uci_status status = decoder.decode(message, codeword, decoder_config);

    // Notify decoded UCI.
    srsran_assert(notifier != nullptr, "Invalid notifier.");
    notifier->on_uci_decoded(message, status);
  }
};

} // namespace srsran