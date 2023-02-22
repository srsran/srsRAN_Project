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

#include "uci_decoder_impl.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"

using namespace srsran;

static uci_status detect_short_block(span<uint8_t>                     output,
                                     span<const log_likelihood_ratio>  input,
                                     short_block_detector*             dec,
                                     const uci_decoder::configuration& config)
{
  modulation_scheme mod = config.modulation;

  bool detection_OK = dec->detect(output, input, mod);

  return detection_OK ? uci_status::valid : uci_status::invalid;
}

uci_status
uci_decoder_impl::decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config)
{
  unsigned msg_size = message.size();
  srsran_assert((msg_size <= MAX_UCI_MSG_LENGTH), "UCI message lengths above 11 bits are not currently supported.");

  // UCI messages between 1 and 11 bits will be handled by the short block decoder.
  uci_status dec_status = detect_short_block(message, llr, decoder.get(), config);

  return dec_status;
}
