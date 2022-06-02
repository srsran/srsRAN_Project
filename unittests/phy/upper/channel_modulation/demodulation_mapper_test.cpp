/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Demodulation mapper unit test.
///
/// The test takes as input vectors containing noisy modulated symbols and the corresponding noise variances. The
/// symbols are demodulated and the resulting bits (both soft and hard versions) are compared with the expected values,
/// also provided by test vectors.

#include "demodulation_mapper_test_data.h"
#include "srsgnb/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsgnb/support/srsgnb_test.h"

#include <random>

using namespace srsgnb;

int main()
{
  std::unique_ptr<demodulation_mapper> demodulator = create_demodulation_mapper();

  for (const auto& test_case : demodulation_mapper_test_data) {
    const modulation_scheme mod = test_case.scheme;

    const unsigned          nof_symbols = test_case.nsymbols;
    const std::vector<cf_t> symbols     = test_case.symbols.read();
    TESTASSERT_EQ(symbols.size(), nof_symbols, "Error reading modulated symbols.");

    const std::vector<float> noise_var = test_case.noise_var.read();
    TESTASSERT_EQ(noise_var.size(), nof_symbols, "Error reading noise variances.");
    TESTASSERT(std::all_of(noise_var.cbegin(), noise_var.cend(), [](float f) { return f > 0; }),
               "Noise variances should take positive values.");

    const unsigned                    nof_bits = nof_symbols * get_bits_per_symbol(mod);
    std::vector<log_likelihood_ratio> soft_bits(nof_bits);
    demodulator->demodulate_soft(soft_bits, symbols, noise_var, mod);

    const std::vector<log_likelihood_ratio> soft_bits_true = test_case.soft_bits.read();
    TESTASSERT_EQ(soft_bits_true.size(), nof_bits, "Error reading soft bits.");

    TESTASSERT(std::equal(soft_bits.cbegin(),
                          soft_bits.cend(),
                          soft_bits_true.cbegin(),
                          [](log_likelihood_ratio a, log_likelihood_ratio b) { return (a == b); }),
               "Soft bits are not sufficiently precise.");

    std::vector<uint8_t> hard_bits(nof_bits);
    std::transform(soft_bits.cbegin(), soft_bits.cend(), hard_bits.begin(), [](log_likelihood_ratio a) {
      return a.to_hard_bit();
    });
    const std::vector<uint8_t> hard_bits_true = test_case.hard_bits.read();
    TESTASSERT_EQ(hard_bits_true.size(), nof_bits, "Error reading hard bits.");
    TESTASSERT(std::equal(hard_bits.cbegin(), hard_bits.cend(), hard_bits_true.cbegin()), "Hard bits do not match.");
  }
}