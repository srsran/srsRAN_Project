/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once
#include "pusch_uci_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/channel_processors/uci_decoder.h"
#include "srsran/ran/uci/uci_constants.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

/// \brief Decodes UCI messages multiplexed in PUSCH.
///
/// This class adapts PUSCH decoder buffer interface to decode UCI messages. The buffer interface is exposed through the
/// method new_transmission().
///
/// The instance is configured when new_transmission() is called. Soft bits are enqueued in \c softbits_buffer with the
/// method on_new_softbits(). The method get_next_block_view() is for getting a view of the next soft bits for avoiding
/// copying data several times.
///
/// The decoding starts upon the call on_end_softbits() which notifies the decoding result.
class pusch_uci_decoder_wrapper : private pusch_decoder_buffer
{
public:
  /// Constructs an instance from an UCI decoder and a maximum number of softbits.
  pusch_uci_decoder_wrapper(uci_decoder& decoder_, unsigned max_nof_softbits) :
    decoder(decoder_), softbits_buffer(max_nof_softbits)
  {
  }

  /// \brief Configures a new transmission.
  /// \param[in] message_length UCI message number of bits.
  /// \param[in] modulation     PUSCH modulation.
  /// \param[in] notifier_      Decoder result notifier.
  /// \return A reference to a PUSCH decoder buffer.
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
