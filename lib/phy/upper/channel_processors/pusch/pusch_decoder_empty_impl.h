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

#pragma once

#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pusch/pusch_constants.h"

namespace srsran {

/// \brief Empty PUSCH decoder implementation.
///
/// Implements the \ref pusch_decoder interface without any decoding capability. The decoder notifies the completion of
/// the data processing when the end of the soft bits is notified.
///
/// It notifies a PUSCH decoder result without any statistics and unmatched CRC.
class pusch_decoder_empty_impl : public pusch_decoder
{
public:
  /// \brief Constructs an empty decoder.
  ///
  /// \param[in] nof_prb    Resource grid maximum number of physical resource blocks.
  /// \param[in] nof_layers Maximum number of layers.
  pusch_decoder_empty_impl(unsigned nof_prb, unsigned nof_layers);

  // See interface for documentation.
  pusch_decoder_buffer& new_data(span<uint8_t>           transport_block,
                                 unique_rx_buffer        rm_buffer,
                                 pusch_decoder_notifier& notifier,
                                 const configuration&    cfg) override;

  // See interface for documentation.
  void set_nof_softbits(units::bits nof_softbits) override;

private:
  /// Implements the decoder buffer interface.
  class decoder_buffer_impl : public pusch_decoder_buffer
  {
  public:
    /// Creates a decoder buffer.
    decoder_buffer_impl(unsigned nof_prb, unsigned nof_layers) :
      softbits_buffer(pusch_constants::get_max_codeword_size(nof_prb, nof_layers).value())
    {
    }

    /// Configures the decoder buffer for a new transmission.
    void new_data(span<uint8_t>           transport_block,
                  unique_rx_buffer        rm_buffer,
                  pusch_decoder_notifier& notifier,
                  const configuration&    cfg);

    // See interface for documentation.
    span<log_likelihood_ratio> get_next_block_view(unsigned int block_size) override;

    // See interface for documentation.
    void on_new_softbits(span<const log_likelihood_ratio> softbits) override;

    // See interface for documentation.
    void on_end_softbits() override;

  private:
    /// Soft bit buffer.
    std::vector<log_likelihood_ratio> softbits_buffer;
    /// Current soft bits buffer.
    unique_rx_buffer rm_buffer;
    /// Current notifier.
    pusch_decoder_notifier* notifier = nullptr;
    /// Set to true if the current transmission is the first transmission and requires to reset the rate matching
    /// buffer.
    bool is_new_data = false;
    /// Counts the number of soft bits in the buffer.
    unsigned softbits_count = 0;
    /// Number of codeblocks of the current transmission.
    unsigned nof_codeblocks = 0;
    /// Full code block size, it is used for setting the rate matching buffer to zero.
    units::bits codeblock_size = units::bits(0);
  };

  /// Buffer instance.
  decoder_buffer_impl decoder_buffer;
};

} // namespace srsran