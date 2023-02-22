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

#include "srsran/phy/upper/channel_coding/short/short_block_detector.h"
#include "srsran/phy/upper/channel_processors/uci_decoder.h"

namespace srsran {

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
    srsran_assert(decoder, "Wrong short block detector.");
  };

  // See interface for the documentation.
  uci_status decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) override;

private:
  /// Pointer to a short block detector.
  std::unique_ptr<short_block_detector> decoder;
};

} // namespace srsran
