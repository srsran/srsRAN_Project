/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  srsgnb_assert((msg_size <= MAX_UCI_MSG_LENGTH), "UCI message lengths above 11 bits are not currently supported.");

  // UCI messages between 1 and 11 bits will be handled by the short block decoder.
  uci_status dec_status = detect_short_block(message, llr, decoder.get(), config);

  return dec_status;
}
