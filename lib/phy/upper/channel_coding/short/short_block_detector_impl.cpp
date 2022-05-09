#include "short_block_detector_impl.h"
#include "short_block_encoder_impl.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

static std::array<std::array<int8_t, MAX_BLOCK_LENGTH>, MAX_NOF_CODEWORDS_2> create_lut()
{
  short_block_encoder_impl encoder;

  std::array<std::array<int8_t, MAX_BLOCK_LENGTH>, MAX_NOF_CODEWORDS_2> table = {};

  // Encode all possible codewords corresponding to "even-valued" messages.
  for (unsigned idx = 0; idx != MAX_NOF_CODEWORDS_2; ++idx) {
    // Generate an "even-valued" message.
    std::array<uint8_t, MAX_MSG_LENGTH> bits = {};
    srsvec::bit_unpack(bits, 2 * idx, MAX_MSG_LENGTH);
    std::reverse(bits.begin(), bits.end());

    // Encode the message. Note that, since the message is longer than 2 bits, all modulation schemes give the same
    // result.
    std::array<uint8_t, MAX_BLOCK_LENGTH> cdwd = {};
    encoder.encode(cdwd, bits, modulation_scheme::BPSK);
    // Save the codeword in the (+1, -1) representation.
    std::transform(cdwd.cbegin(), cdwd.cend(), table[idx].begin(), [](uint8_t a) { return (1 - 2 * a); });
  }
  return table;
};

const std::array<std::array<int8_t, MAX_BLOCK_LENGTH>, MAX_NOF_CODEWORDS_2> short_block_detector_impl::DETECT_TABLE =
    create_lut();

static void validate_spans(span<uint8_t> output, span<const int8_t> input, unsigned bits_per_symbol)
{
  unsigned in_size  = input.size();
  unsigned out_size = output.size();
  srsran_assert((out_size > 0) && (out_size <= MAX_MSG_LENGTH), "The output length should be between 1 and 11 bits.");
  if (out_size > 2) {
    srsran_assert(in_size >= MAX_BLOCK_LENGTH, "Invalid output length.");
  } else {
    unsigned min_in_length = 0;
    if (out_size == 1) {
      // Input length must be no less than the number of bits per symbol of the block modulation.
      min_in_length = bits_per_symbol;
    } else {
      // Output length must be no less than three times the number of bits per symbol of the block modulation.
      min_in_length = 3 * bits_per_symbol;
    }
    srsran_assert(in_size >= min_in_length, "Invalid input length.");
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
  return (MAX_BLOCK_LENGTH - 1) * max_metric / (MAX_BLOCK_LENGTH * in_norm_sqr - max_metric);
}

// Recovers the original short codeblock from its rate-matched version.
static void rate_dematch(span<int8_t> output, span<const int8_t> input)
{
  unsigned output_size = output.size();
  unsigned input_size  = input.size();
  srsran_assert(output_size <= input_size, "Output size cannot be larger than input size.");

  std::fill(output.begin(), output.end(), 0);
  for (unsigned idx = 0; idx != input_size; ++idx) {
    output[idx % output_size] = saturated_sum(output[idx % output_size], input[idx], INT8_MAX);
  }
};

bool short_block_detector_impl::detect(span<uint8_t> output, span<const int8_t> input, modulation_scheme mod)
{
  unsigned bits_per_symbol = get_bits_per_symbol(mod);
  validate_spans(output, input, bits_per_symbol);

  static_vector<int8_t, MAX_BLOCK_LENGTH> tmp = {};

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
};

std::unique_ptr<short_block_detector> srsgnb::create_short_block_detector()
{
  return std::make_unique<short_block_detector_impl>();
}