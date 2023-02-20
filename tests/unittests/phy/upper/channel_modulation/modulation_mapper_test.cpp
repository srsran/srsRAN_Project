/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "modulation_mapper_test_data.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsran;

static constexpr float assert_max_err = 1e-6;

static void assert_symbols(span<const cf_t> symbols, span<const cf_t> expected_symbols)
{
  TESTASSERT_EQ(symbols.size(), expected_symbols.size(), "Wrong number of modulated symbols.");

  for (unsigned i = 0; i < symbols.size(); ++i) {
    float err = std::abs(symbols[i] - expected_symbols[i]);
    TESTASSERT(err < assert_max_err, "Mismatched value {} but expected {}", symbols[i], expected_symbols[i]);
  }
}

int main()
{
  std::shared_ptr<channel_modulation_factory> factory = create_channel_modulation_sw_factory();
  TESTASSERT(factory);

  std::unique_ptr<modulation_mapper> modulator = factory->create_modulation_mapper();
  TESTASSERT(modulator);

  for (const test_case_t& test_case : modulation_mapper_test_data) {
    // Load input data
    const std::vector<uint8_t> testvector_data = test_case.data.read();

    dynamic_bit_buffer packed_data(testvector_data.size());
    srsvec::bit_pack(packed_data, testvector_data);

    // Modulate
    srsran::srsvec::aligned_vec<cf_t> symbols(test_case.nsymbols);
    modulator->modulate(symbols, packed_data, test_case.scheme);

    // Load expected symbols and verify the result
    const std::vector<cf_t> testvector_symbols = test_case.symbols.read();
    span<const cf_t>        expected_symbols{testvector_symbols};
    assert_symbols(symbols, expected_symbols);
  }
  return 0;
}