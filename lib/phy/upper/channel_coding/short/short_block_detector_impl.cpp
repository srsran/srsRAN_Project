#include "short_block_detector_impl.h"
#include "short_block_encoder_impl.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

static std::array<std::array<int8_t, MAX_IN_LENGTH>, MAX_NOF_CODEWORDS_2> create_lut()
{
  short_block_encoder_impl encoder;

  std::array<std::array<int8_t, MAX_IN_LENGTH>, MAX_NOF_CODEWORDS_2> table = {};

  // Encode all possible codewords corresponding to "even-valued" messages.
  for (unsigned idx = 0; idx != MAX_NOF_CODEWORDS_2; ++idx) {
    // Generate an "even-valued" message.
    std::array<uint8_t, MAX_OUT_LENGTH> bits = {};
    srsvec::bit_unpack(bits, 2 * idx, MAX_OUT_LENGTH);
    std::reverse(bits.begin(), bits.end());

    // Encode the message. Note that, since the message is longer than 2 bits, all modulation schemes give the same
    // result.
    std::array<uint8_t, MAX_IN_LENGTH> cdwd = {};
    encoder.encode(cdwd, bits, modulation_scheme::BPSK);
    // Save the codeword in the (+1, -1) representation.
    std::transform(cdwd.cbegin(), cdwd.cend(), table[idx].begin(), [](uint8_t a) { return (1 - 2 * a); });
  }
  return table;
};

const std::array<std::array<int8_t, MAX_IN_LENGTH>, MAX_NOF_CODEWORDS_2> short_block_detector_impl::DETECT_TABLE =
    create_lut();

static void validate_spans(span<uint8_t> output, span<const int8_t> input)
{
  unsigned in_size  = input.size();
  unsigned out_size = output.size();
  srsran_assert((out_size > 0) && (out_size <= MAX_OUT_LENGTH), "The output length should be between 1 and 11 bits.");
  if (out_size > 2) {
    srsran_assert(in_size == MAX_IN_LENGTH, "Invalid output length.");
  } else {
    std::array<unsigned, 5> in_lengths = {};
    if (out_size == 1) {
      // Output length must be equal to the number of bits per symbol of the block modulation.
      in_lengths = {1, 2, 4, 6, 8};
    } else {
      // Output length must be equal to three times the number of bits per symbol of the block modulation.
      in_lengths = {3, 6, 12, 18, 24};
    }
    auto this_length = {in_size};
    srsran_assert(std::includes(in_lengths.cbegin(), in_lengths.cend(), this_length.begin(), this_length.end()),
                  "Invalid input length.");
  }
}

// ML detection for 2-bit messages.
static double detect_2(span<uint8_t> output, span<const int8_t> input)
{
  constexpr unsigned        NOF_BITS = 3;
  std::array<int, NOF_BITS> llr      = {};

  unsigned in_size = input.size();
  if (in_size == NOF_BITS) {
    std::copy(input.begin(), input.end(), llr.begin());
  } else {
    // in_size > NOF_BITS is equivalent to modulation of order higher than 1: combine repeated symbols.
    unsigned step = in_size / 3 - 2;
    llr[0]        = input[0] + input[step + 3];
    llr[1]        = input[1] + input[2 * step + 4];
    llr[2]        = input[step + 2] + input[2 * step + 5];
  }

  // All possible 2-bit codewords (including redundancy bit).
  constexpr std::array<std::array<int, 3>, 4> TABLE2 = {{{1, 1, 1}, {-1, 1, -1}, {1, -1, -1}, {-1, -1, 1}}};

  unsigned max_idx    = 0;
  double   max_metric = std::numeric_limits<double>::min();
  // Brute-force ML detector: correlate all codewords with the LLRs and pick the best one.
  for (unsigned cdwd_idx = 0; cdwd_idx != 4; ++cdwd_idx) {
    int metric = srsvec::dot_prod(span<const int>(llr), span<const int>(TABLE2[cdwd_idx]), 0);
    if (metric > max_metric) {
      max_metric = metric;
      max_idx    = cdwd_idx;
    }
  }

  output[0] = max_idx & 1U;
  output[1] = (max_idx >> 1U) & 1U;

  // TODO(david): this is not really working, 3 symbols are not enough for a meaningful GLRT detector.
  max_metric *= max_metric;
  int in_norm_sqr = srsvec::dot_prod(span<const int>(llr), span<const int>(llr), 0);
  return 2.0 * max_metric / (3.0 * in_norm_sqr - max_metric);
}

// ML detection for (3-to-11)-bit messages.
double short_block_detector_impl::detect_3_11(span<uint8_t> output, span<const int8_t> input)
{
  unsigned out_size      = output.size();
  unsigned nof_codewords = (1U << (out_size - 1));

  unsigned max_idx    = 0;
  double   max_metric = std::numeric_limits<double>::min();
  uint8_t  bit0       = 0U;
  // Brute-force ML detector: correlate all codewords with the LLRs and pick the best one.
  for (unsigned cdwd_idx = 0; cdwd_idx != nof_codewords; ++cdwd_idx) {
    int metric     = srsvec::dot_prod(input, span<const int8_t>(DETECT_TABLE[cdwd_idx]), 0);
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
  int in_norm_sqr = srsvec::dot_prod(input, input, 0);
  return (MAX_IN_LENGTH - 1) * max_metric / (MAX_IN_LENGTH * in_norm_sqr - max_metric);
}

bool short_block_detector_impl::detect(span<uint8_t> output, span<const int8_t> input)
{
  validate_spans(output, input);

  double   max_metric = 0;
  unsigned out_size   = output.size();
  switch (out_size) {
    case 1:
      output[0]  = (input[0] > 0) ? 0 : 1;
      max_metric = 1;
      break;
    case 2:
      max_metric = detect_2(output, input);
      break;
    default:
      max_metric = detect_3_11(output, input);
  }

  // Detection threshold values computed with the generalized likelihood ratio test.
  // TODO(david): Thresholds for the 1- and 2-bit cases are not meaningful.
  constexpr std::array<double, MAX_OUT_LENGTH> THRESHOLDS = {0, 0, 12, 14, 16, 18, 20, 22, 24, 26, 29};
  return (max_metric > THRESHOLDS[out_size - 1]);
};

void short_block_detector_impl::rate_dematch(span<int8_t> output, span<const int8_t> input)
{
  unsigned output_size = output.size();
  unsigned input_size  = input.size();
  srsran_assert(output_size <= input_size, "Output size cannot be larger than input size.");

  std::fill(output.begin(), output.end(), 0);
  for (unsigned idx = 0; idx != input_size; ++idx) {
    output[idx % output_size] = saturated_sum(output[idx % output_size], input[idx], INT8_MAX);
  }
};

std::unique_ptr<short_block_detector> srsgnb::create_short_block_detector()
{
  return std::make_unique<short_block_detector_impl>();
}