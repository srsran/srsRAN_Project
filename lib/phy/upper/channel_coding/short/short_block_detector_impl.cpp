/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "short_block_detector_impl.h"
#include "short_block_encoder_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/uci/uci_info.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/fill.h"

using namespace srsran;

static std::array<std::array<int8_t, MAX_BLOCK_LENGTH>, MAX_NOF_CODEWORDS_2> create_lut()
{
  short_block_encoder_impl encoder;

  std::array<std::array<int8_t, MAX_BLOCK_LENGTH>, MAX_NOF_CODEWORDS_2> table;

  // Encode all possible codewords corresponding to "even-valued" messages.
  for (unsigned idx = 0; idx != MAX_NOF_CODEWORDS_2; ++idx) {
    // Generate an "even-valued" message.
    std::array<uint8_t, MAX_MSG_LENGTH> bits;
    srsvec::bit_unpack(bits, 2 * idx, MAX_MSG_LENGTH);
    std::reverse(bits.begin(), bits.end());

    // Encode the message. Note that, since the message is longer than 2 bits, all modulation schemes give the same
    // result.
    std::array<uint8_t, MAX_BLOCK_LENGTH> cdwd;
    encoder.encode(cdwd, bits, modulation_scheme::BPSK);
    // Save the codeword in the (+1, -1) representation.
    std::transform(cdwd.cbegin(), cdwd.cend(), table[idx].begin(), [](uint8_t a) { return (1 - 2 * a); });
  }
  return table;
}

const std::array<std::array<int8_t, MAX_BLOCK_LENGTH>, MAX_NOF_CODEWORDS_2> short_block_detector_impl::DETECT_TABLE =
    create_lut();

static bool validate_spans(span<uint8_t> output, span<const log_likelihood_ratio> input, unsigned bits_per_symbol)
{
  unsigned in_size  = input.size();
  unsigned out_size = output.size();

  // The output length should be between 1 and 11 bits.
  if ((out_size == 0) || (out_size > MAX_MSG_LENGTH)) {
    return false;
  }

  // Count the number of input bits that are not zero.
  unsigned non_zero_count =
      std::count_if(input.begin(), input.end(), [](log_likelihood_ratio elem) { return elem != 0; });

  // The input number of non-zero bits must be equal to or greater than the minimum number of bits.
  unsigned min_input_bits = calculate_uci_min_encoded_bits(out_size);
  if (non_zero_count < min_input_bits) {
    return false;
  }

  // For 1 and 2 bits, the number of input bits bust be equal to or greater than the modulation order.
  if ((out_size <= 2) && (in_size < bits_per_symbol)) {
    return false;
  }

  return true;
}

// ML detection for 2-bit messages.
static double detect_2(span<uint8_t> output, span<const log_likelihood_ratio> input)
{
  constexpr unsigned        NOF_BITS   = 3;
  std::array<int, NOF_BITS> llr_as_int = {};

  unsigned in_size = input.size();
  if (in_size == NOF_BITS) {
    std::transform(input.begin(), input.end(), llr_as_int.begin(), [](log_likelihood_ratio a) { return a.to_int(); });
  } else {
    // in_size > NOF_BITS is equivalent to modulation of order higher than 1: combine repeated symbols.
    unsigned step = in_size / 3 - 2;
    llr_as_int[0] = input[0].to_int() + input[step + 3].to_int();
    llr_as_int[1] = input[1].to_int() + input[2 * step + 4].to_int();
    llr_as_int[2] = input[step + 2].to_int() + input[2 * step + 5].to_int();
  }

  // All possible 2-bit codewords (including redundancy bit).
  constexpr std::array<std::array<int, 3>, 4> TABLE2 = {{{1, 1, 1}, {-1, 1, -1}, {1, -1, -1}, {-1, -1, 1}}};

  unsigned max_idx    = 0;
  double   max_metric = std::numeric_limits<double>::min();
  // Brute-force ML detector: correlate all codewords with the LLRs and pick the best one.
  for (unsigned cdwd_idx = 0; cdwd_idx != 4; ++cdwd_idx) {
    int metric = srsvec::dot_prod(llr_as_int, TABLE2[cdwd_idx], 0);
    if (metric > max_metric) {
      max_metric = metric;
      max_idx    = cdwd_idx;
    }
  }

  output[0] = max_idx & 1U;
  output[1] = (max_idx >> 1U) & 1U;

  // TODO(david): this is not really working, 3 symbols are not enough for a meaningful GLRT detector.
  max_metric *= max_metric;
  int in_norm_sqr = srsvec::dot_prod(llr_as_int, llr_as_int, 0);
  return 2.0 * max_metric / (3.0 * in_norm_sqr - max_metric);
}

// ML detection for (3-to-11)-bit messages.
double short_block_detector_impl::detect_3_11(span<uint8_t> output, span<const log_likelihood_ratio> input)
{
  unsigned out_size      = output.size();
  unsigned nof_codewords = (1U << (out_size - 1));

  unsigned max_idx    = 0;
  double   max_metric = std::numeric_limits<double>::min();
  uint8_t  bit0       = 0U;
  // Brute-force ML detector: correlate all codewords with the LLRs and pick the best one.
  for (unsigned cdwd_idx = 0; cdwd_idx != nof_codewords; ++cdwd_idx) {
    int metric     = log_likelihood_ratio::dot_prod_sign(input, DETECT_TABLE[cdwd_idx]);
    int metric_abs = std::abs(metric);
    if (metric_abs > max_metric) {
      max_metric = metric_abs;
      max_idx    = cdwd_idx;
      bit0       = static_cast<uint8_t>(metric < 0);
    }
  }

  // Recover the message from the index of the codeword with the highest correlation. Recall that only "even-valued"
  // messages have been correlated, the "odd-values" ones are those with negative correlation and, in turn, bit0 = 1.
  srsvec::bit_unpack(output, 2 * max_idx + bit0, output.size());
  std::reverse(output.begin(), output.end());

  // GLRT detector metric.
  max_metric *= max_metric;
  int in_norm_sqr = log_likelihood_ratio::norm_squared(input);
  return (MAX_BLOCK_LENGTH - 1) * max_metric / (MAX_BLOCK_LENGTH * in_norm_sqr - max_metric);
}

// Recovers the original short codeblock from its rate-matched version.
static void rate_dematch(span<log_likelihood_ratio> output, span<const log_likelihood_ratio> input)
{
  // Copy the first samples in common.
  unsigned nof_copy = std::min(input.size(), output.size());
  srsvec::copy(output.first(nof_copy), input.first(nof_copy));

  // Pad the output with zeros if longer than the input.
  if (input.size() <= output.size()) {
    srsvec::fill(output.last(output.size() - nof_copy), 0);
    return;
  }

  // Combine the rest of LLRs in blocks.
  input = input.last(input.size() - output.size());
  while (!input.empty()) {
    // Determine the block size.
    unsigned block_size = std::min(output.size(), input.size());

    // Combine block of LLR.
    log_likelihood_ratio::sum(output.first(block_size), output.first(block_size), input.first(block_size));

    // Advance input.
    input = input.last(input.size() - block_size);
  }
}

bool short_block_detector_impl::detect(span<uint8_t>                    output,
                                       span<const log_likelihood_ratio> input,
                                       modulation_scheme                mod)
{
  // If the spans sizes are invalid, the result is invalid.
  unsigned bits_per_symbol = get_bits_per_symbol(mod);
  if (!validate_spans(output, input, bits_per_symbol)) {
    std::fill(output.begin(), output.end(), 1);
    return false;
  }

  static_vector<log_likelihood_ratio, MAX_BLOCK_LENGTH> tmp;

  double   max_metric = 0;
  unsigned out_size   = output.size();
  switch (out_size) {
    case 1:
      tmp.resize(bits_per_symbol);
      rate_dematch(tmp, input);
      output[0]  = (tmp[0] > 0) ? 0 : 1;
      max_metric = 1;
      break;
    case 2:
      tmp.resize(3UL * bits_per_symbol);
      rate_dematch(tmp, input);
      max_metric = detect_2(output, tmp);
      break;
    default:
      tmp.resize(MAX_BLOCK_LENGTH);
      rate_dematch(tmp, input);
      max_metric = detect_3_11(output, tmp);
  }

  // Detection threshold values computed with the generalized likelihood ratio test.
  // TODO(david): Thresholds for the 1- and 2-bit cases are not meaningful.
  constexpr std::array<double, MAX_MSG_LENGTH> THRESHOLDS = {0, 0, 12, 14, 16, 18, 20, 22, 24, 26, 29};
  return (max_metric > THRESHOLDS[out_size - 1]);
}
