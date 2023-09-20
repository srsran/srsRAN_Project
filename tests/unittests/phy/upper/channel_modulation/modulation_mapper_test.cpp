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

#include "modulation_mapper_test_data.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

static constexpr float assert_max_err = 1e-3;

template <typename ComplexType>
static void assert_symbols(span<const ComplexType> symbols, span<const cf_t> expected_symbols, float scaling = 1.0F)
{
  TESTASSERT_EQ(symbols.size(), expected_symbols.size(), "Wrong number of modulated symbols.");

  for (unsigned i = 0; i < symbols.size(); ++i) {
    cf_t symbol =
        cf_t(scaling * static_cast<float>(symbols[i].real()), scaling * static_cast<float>(symbols[i].imag()));
    float err = std::abs(symbol - expected_symbols[i]);
    TESTASSERT(err < assert_max_err, "Mismatched value {} but expected {}", symbol, expected_symbols[i]);
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

    // Load expected symbols in floating point.
    const std::vector<cf_t> testvector_symbols = test_case.symbols.read();
    span<const cf_t>        expected_symbols{testvector_symbols};

    dynamic_bit_buffer packed_data(testvector_data.size());
    srsvec::bit_pack(packed_data, testvector_data);

    // Modulate in complex float.
    srsran::srsvec::aligned_vec<cf_t> symbols_cf(test_case.nsymbols);
    modulator->modulate(symbols_cf, packed_data, test_case.scheme);
    assert_symbols<cf_t>(symbols_cf, expected_symbols);

    // Modulate in complex i8.
    srsran::srsvec::aligned_vec<ci8_t> symbols_ci8(test_case.nsymbols);
    float                              scale_ci8 = modulator->modulate(symbols_ci8, packed_data, test_case.scheme);
    assert_symbols<ci8_t>(symbols_ci8, expected_symbols, scale_ci8);
  }
  return 0;
}