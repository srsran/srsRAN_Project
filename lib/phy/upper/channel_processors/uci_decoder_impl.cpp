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
#include "srsran/ran/uci/uci_info.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

// Maximum UCI message size for block detector.
static constexpr unsigned MAX_UCI_BLOCK_SIZE = 11;

uci_status uci_decoder_impl::detect_short_block(span<uint8_t>                     output,
                                                span<const log_likelihood_ratio>  input,
                                                const uci_decoder::configuration& config)
{
  modulation_scheme mod = config.modulation;

  bool detection_OK = block_detector->detect(output, input, mod);

  return (detection_OK ? uci_status::valid : uci_status::invalid);
}

uci_status uci_decoder_impl::decode_codeword_polar(span<uint8_t> output, span<const log_likelihood_ratio> input)
{
  unsigned message_length  = output.size();
  unsigned codeword_length = input.size();

  // Get the number of UCI code blocks.
  unsigned nof_codeblocks = get_nof_uci_codeblocks(message_length, codeword_length);

  // Get the number of CRC bits per codeblock.
  unsigned crc_size = get_uci_crc_size(message_length);

  // Select CRC calculator.
  crc_calculator& crc = (crc_size == 11) ? *crc11 : *crc6;

  // Decode first code block.
  unsigned   codeblock0_size     = message_length / nof_codeblocks;
  unsigned   enc_codeblock0_size = codeword_length / nof_codeblocks;
  unsigned   nof_filler_bits     = message_length % nof_codeblocks;
  uci_status status =
      decode_codeblock_polar(output.first(codeblock0_size), input.first(enc_codeblock0_size), nof_filler_bits, crc);

  // Decode second codeblock if present.
  if ((status == uci_status::valid) && (nof_codeblocks > 1)) {
    unsigned   codeblock1_size     = divide_ceil(message_length, nof_codeblocks);
    unsigned   enc_codeblock1_size = (codeword_length / nof_codeblocks);
    uci_status cb1_status          = decode_codeblock_polar(output.subspan(codeblock0_size, codeblock1_size),
                                                   input.subspan(enc_codeblock0_size, enc_codeblock1_size),
                                                   0,
                                                   crc);

    // If the second codeblock is invalid, then the resultant status is invalid.
    if (cb1_status != uci_status::valid) {
      status = cb1_status;
    }
  }

  return status;
}

uci_status uci_decoder_impl::decode_codeblock_polar(span<uint8_t>                    message,
                                                    span<const log_likelihood_ratio> softbits,
                                                    unsigned                         nof_filler_bits,
                                                    crc_calculator&                  crc)
{
  // Select CRC size.
  unsigned crc_size = get_crc_size(crc.get_generator_poly());

  // Calculate total message size.
  unsigned message_size = message.size() + crc_size + nof_filler_bits;

  // Setup polar code.
  code->set(message_size, softbits.size(), 10, polar_code_ibil::present);

  // Perform rate dematch.
  span<log_likelihood_ratio> rm_buffer = span<log_likelihood_ratio>(temp_rm_buffer).first(code->get_N());
  rate_dematcher->rate_dematch(rm_buffer, softbits, *code);

  // Actual decode.
  span<uint8_t> decoder_buffer = span<uint8_t>(temp_decoder_buffer).first(code->get_N());
  decoder->decode(decoder_buffer, rm_buffer, *code);

  // Deallocate data.
  span<uint8_t> deallocator_buffer = span<uint8_t>(temp_deallocator_buffer).first(message_size);
  deallocator->deallocate(deallocator_buffer, decoder_buffer, *code);

  // Calculate CRC.
  crc_calculator_checksum_t checksum = crc.calculate_bit(deallocator_buffer.first(message_size));

  // Determine if the decoding is successful.
  uci_status status = (checksum == 0) ? uci_status::valid : uci_status::invalid;

  // Copy message without CRC.
  srsvec::copy(message, deallocator_buffer.subspan(nof_filler_bits, message_size - nof_filler_bits - crc_size));

  return status;
}

uci_status
uci_decoder_impl::decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config)
{
  unsigned msg_size = message.size();

  // UCI messages between 1 and 11 bits will be handled by the short block decoder.
  if (msg_size <= MAX_UCI_BLOCK_SIZE) {
    return detect_short_block(message, llr, config);
  }

  return decode_codeword_polar(message, llr);
}
