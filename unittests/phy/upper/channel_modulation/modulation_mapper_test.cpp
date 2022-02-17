/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <cassert>
#include <random>

static std::mt19937 rgen(0);

using namespace srsgnb;

static const float assert_max_err = 1e-6;

static void modulate_generic(const span<uint8_t>& bits, span<cf_t> symbols, modulation_scheme scheme)
{
  unsigned Qm    = 0;
  float    scale = 0;
  switch (scheme) {
    case modulation_scheme::BPSK:
      Qm    = 1;
      scale = 1.0F / std::sqrt(2.0F);
      break;
    case modulation_scheme::QPSK:
      Qm    = 2;
      scale = 1.0F / std::sqrt(2.0F);
      break;
    case modulation_scheme::QAM16:
      Qm    = 4;
      scale = 1.0F / std::sqrt(10.0F);
      break;
    case modulation_scheme::QAM64:
      Qm    = 6;
      scale = 1.0F / std::sqrt(42.0F);
      break;
    case modulation_scheme::QAM256:
      Qm    = 8;
      scale = 1.0F / std::sqrt(170.0F);
      break;
  }

  assert(Qm != 0);
  assert(std::isnormal(Qm));
  assert(bits.size() / Qm == symbols.size());

  // For each symbol
  for (unsigned symb_idx = 0; symb_idx != symbols.size(); ++symb_idx) {
    float offset = -1;
    float real   = 0;
    float imag   = 0;

    // BPSK is an exception
    if (Qm == 1) {
      // Set symbol value
      symbols[symb_idx] = (bits[symb_idx] == 0) ? cf_t{scale, scale} : cf_t{-scale, -scale};

      continue;
    }

    // For each pair of bits in the symbol
    for (uint32_t j = 0; j < Qm / 2; j++) {
      // Calculate bit indexes for Real and Imaginary
      unsigned real_bit_idx = Qm * symb_idx + (Qm - 2 * j - 2);
      unsigned imag_bit_idx = Qm * symb_idx + (Qm - 2 * j - 1);

      // Apply offset
      real += offset;
      imag += offset;

      // Increase offset per symbol
      offset *= 2;

      // Apply modulation
      real *= (bits[real_bit_idx] != 0) ? +1 : -1;
      imag *= (bits[imag_bit_idx] != 0) ? +1 : -1;
    }

    // Set symbol value
    symbols[symb_idx] = {real * scale, imag * scale};
  }
}

void test_modulator(std::size_t nsymbols, modulation_scheme scheme)
{
  unsigned                                     Qm = static_cast<unsigned>(scheme);
  std::uniform_int_distribution<unsigned char> dist(0, 1);

  // Create data bits
  srsgnb::srsvec::aligned_vec<uint8_t> data(Qm * nsymbols);

  // Fill data with random bits
  for (unsigned char& v : data) {
    v = dist(rgen);
  }

  // Modulate
  srsgnb::srsvec::aligned_vec<cf_t>  symbols(nsymbols);
  std::unique_ptr<modulation_mapper> modulator = create_modulation_mapper();
  modulator->modulate(data, symbols, scheme);

  // Generate golden sequence
  srsgnb::srsvec::aligned_vec<cf_t> symbols_gold(nsymbols);
  modulate_generic(data, symbols_gold, scheme);

  // Assert
  for (unsigned i = 0; i != nsymbols; ++i) {
    // Calculate error
    float err = std::abs(symbols_gold[i] - symbols[i]);

    // Print for debugging purposes
    //    printf("Qm=%d; gold=%+.6f%+.6f; symbol=%+.6f%+.6f; err=%f\n",
    //           Qm,
    //           symbols_gold[i].real(),
    //           symbols_gold[i].imag(),
    //           symbols[i].real(),
    //           symbols[i].imag(),
    //           err);

    // Assert maximum error
    assert(err < assert_max_err);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {257, 997};

  for (std::size_t N : sizes) {
    test_modulator(N, modulation_scheme::BPSK);
    test_modulator(N, modulation_scheme::QPSK);
    test_modulator(N, modulation_scheme::QAM16);
    test_modulator(N, modulation_scheme::QAM64);
    test_modulator(N, modulation_scheme::QAM256);
  }
}
