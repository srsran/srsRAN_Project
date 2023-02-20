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

#include "srsgnb/phy/upper/channel_coding/short/short_block_detector.h"
#include "srsgnb/phy/upper/channel_processors/uci_decoder.h"

namespace srsgnb {

/// Currently-supported maximum length.
static constexpr unsigned MAX_UCI_MSG_LENGTH = 11;

/// Implementation of the UCI decoder.
class uci_decoder_impl : public uci_decoder
{
public:
  /// \brief UCI decoder constructor.
  ///
  /// Sets up the internal short block decoder.
  ///
  /// \param[in] dec   Pointer to a short block detector object.
  uci_decoder_impl(std::unique_ptr<short_block_detector> dec) : decoder(std::move(dec))
  {
    srsgnb_assert(decoder, "Wrong short block detector.");
  };

  // See interface for the documentation.
  uci_status decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) override;

private:
  /// Pointer to a short block detector.
  std::unique_ptr<short_block_detector> decoder;
};

} // namespace srsgnb
