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

#include "modulation_mapper_impl.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;

#define BPSK_LEVEL M_SQRT1_2

#define QPSK_LEVEL M_SQRT1_2

#define QAM16_LEVEL_1 (1.0f / sqrtf(10.0f))
#define QAM16_LEVEL_2 (3.0f / sqrtf(10.0f))

#define QAM64_LEVEL_1 (1.0f / sqrtf(42.0f))
#define QAM64_LEVEL_2 (3.0f / sqrtf(42.0f))
#define QAM64_LEVEL_3 (5.0f / sqrtf(42.0f))
#define QAM64_LEVEL_4 (7.0f / sqrtf(42.0f))

modulation_mapper_impl::bpsk_table_s::bpsk_table_s()
{
  // BPSK constellation:
  //    Q
  //    |  0
  //---------> I
  // 1  |
  //
  // see [3GPP TS 38.211 version 15.8.0, Section 5.1.2]
  table.resize(2);
  table[0] = {BPSK_LEVEL, BPSK_LEVEL};
  table[1] = {-BPSK_LEVEL, -BPSK_LEVEL};
  Qm       = 1;
}

modulation_mapper_impl::qpsk_table_s::qpsk_table_s()
{
  // QPSK constellation:
  //     Q
  // 10  |  00
  //-----------> I
  // 11  |  01
  //
  // see [3GPP TS 38.211 version 15.8.0, Section 5.1.3]
  table.resize(4);
  table[0] = {QPSK_LEVEL, +QPSK_LEVEL};
  table[1] = {QPSK_LEVEL, -QPSK_LEVEL};
  table[2] = {-QPSK_LEVEL, +QPSK_LEVEL};
  table[3] = {-QPSK_LEVEL, -QPSK_LEVEL};
  Qm       = 2;
}

modulation_mapper_impl::qam16_table_s::qam16_table_s()
{
  // 16QAM constellation:
  //                Q
  //  1011  1001  |   0001  0011
  //  1010  1000  |   0000  0010
  //---------------------------------> I
  //  1110    1100  |  0100  0110
  //  1111    1101  |  0101  0111
  //
  // see [3GPP TS 38.211 version 15.8.0, Section 5.1.4]
  table.resize(16);
  table[0]  = {QAM16_LEVEL_1, +QAM16_LEVEL_1};
  table[1]  = {QAM16_LEVEL_1, +QAM16_LEVEL_2};
  table[2]  = {QAM16_LEVEL_2, +QAM16_LEVEL_1};
  table[3]  = {QAM16_LEVEL_2, +QAM16_LEVEL_2};
  table[4]  = {QAM16_LEVEL_1, -QAM16_LEVEL_1};
  table[5]  = {QAM16_LEVEL_1, -QAM16_LEVEL_2};
  table[6]  = {QAM16_LEVEL_2, -QAM16_LEVEL_1};
  table[7]  = {QAM16_LEVEL_2, -QAM16_LEVEL_2};
  table[8]  = {-QAM16_LEVEL_1, +QAM16_LEVEL_1};
  table[9]  = {-QAM16_LEVEL_1, +QAM16_LEVEL_2};
  table[10] = {-QAM16_LEVEL_2, +QAM16_LEVEL_1};
  table[11] = {-QAM16_LEVEL_2, +QAM16_LEVEL_2};
  table[12] = {-QAM16_LEVEL_1, -QAM16_LEVEL_1};
  table[13] = {-QAM16_LEVEL_1, -QAM16_LEVEL_2};
  table[14] = {-QAM16_LEVEL_2, -QAM16_LEVEL_1};
  table[15] = {-QAM16_LEVEL_2, -QAM16_LEVEL_2};
  Qm        = 4;
}

modulation_mapper_impl::qam64_table_s::qam64_table_s()
{
  // 64QAM constellation:
  // see [3GPP TS 38.211 version 15.8.0, Section 5.1.5]
  table.resize(64);
  table[0]  = {QAM64_LEVEL_2, +QAM64_LEVEL_2};
  table[1]  = {QAM64_LEVEL_2, +QAM64_LEVEL_1};
  table[2]  = {QAM64_LEVEL_1, +QAM64_LEVEL_2};
  table[3]  = {QAM64_LEVEL_1, +QAM64_LEVEL_1};
  table[4]  = {QAM64_LEVEL_2, +QAM64_LEVEL_3};
  table[5]  = {QAM64_LEVEL_2, +QAM64_LEVEL_4};
  table[6]  = {QAM64_LEVEL_1, +QAM64_LEVEL_3};
  table[7]  = {QAM64_LEVEL_1, +QAM64_LEVEL_4};
  table[8]  = {QAM64_LEVEL_3, +QAM64_LEVEL_2};
  table[9]  = {QAM64_LEVEL_3, +QAM64_LEVEL_1};
  table[10] = {QAM64_LEVEL_4, +QAM64_LEVEL_2};
  table[11] = {QAM64_LEVEL_4, +QAM64_LEVEL_1};
  table[12] = {QAM64_LEVEL_3, +QAM64_LEVEL_3};
  table[13] = {QAM64_LEVEL_3, +QAM64_LEVEL_4};
  table[14] = {QAM64_LEVEL_4, +QAM64_LEVEL_3};
  table[15] = {QAM64_LEVEL_4, +QAM64_LEVEL_4};
  table[16] = {QAM64_LEVEL_2, -QAM64_LEVEL_2};
  table[17] = {QAM64_LEVEL_2, -QAM64_LEVEL_1};
  table[18] = {QAM64_LEVEL_1, -QAM64_LEVEL_2};
  table[19] = {QAM64_LEVEL_1, -QAM64_LEVEL_1};
  table[20] = {QAM64_LEVEL_2, -QAM64_LEVEL_3};
  table[21] = {QAM64_LEVEL_2, -QAM64_LEVEL_4};
  table[22] = {QAM64_LEVEL_1, -QAM64_LEVEL_3};
  table[23] = {QAM64_LEVEL_1, -QAM64_LEVEL_4};
  table[24] = {QAM64_LEVEL_3, -QAM64_LEVEL_2};
  table[25] = {QAM64_LEVEL_3, -QAM64_LEVEL_1};
  table[26] = {QAM64_LEVEL_4, -QAM64_LEVEL_2};
  table[27] = {QAM64_LEVEL_4, -QAM64_LEVEL_1};
  table[28] = {QAM64_LEVEL_3, -QAM64_LEVEL_3};
  table[29] = {QAM64_LEVEL_3, -QAM64_LEVEL_4};
  table[30] = {QAM64_LEVEL_4, -QAM64_LEVEL_3};
  table[31] = {QAM64_LEVEL_4, -QAM64_LEVEL_4};
  table[32] = {-QAM64_LEVEL_2, +QAM64_LEVEL_2};
  table[33] = {-QAM64_LEVEL_2, +QAM64_LEVEL_1};
  table[34] = {-QAM64_LEVEL_1, +QAM64_LEVEL_2};
  table[35] = {-QAM64_LEVEL_1, +QAM64_LEVEL_1};
  table[36] = {-QAM64_LEVEL_2, +QAM64_LEVEL_3};
  table[37] = {-QAM64_LEVEL_2, +QAM64_LEVEL_4};
  table[38] = {-QAM64_LEVEL_1, +QAM64_LEVEL_3};
  table[39] = {-QAM64_LEVEL_1, +QAM64_LEVEL_4};
  table[40] = {-QAM64_LEVEL_3, +QAM64_LEVEL_2};
  table[41] = {-QAM64_LEVEL_3, +QAM64_LEVEL_1};
  table[42] = {-QAM64_LEVEL_4, +QAM64_LEVEL_2};
  table[43] = {-QAM64_LEVEL_4, +QAM64_LEVEL_1};
  table[44] = {-QAM64_LEVEL_3, +QAM64_LEVEL_3};
  table[45] = {-QAM64_LEVEL_3, +QAM64_LEVEL_4};
  table[46] = {-QAM64_LEVEL_4, +QAM64_LEVEL_3};
  table[47] = {-QAM64_LEVEL_4, +QAM64_LEVEL_4};
  table[48] = {-QAM64_LEVEL_2, -QAM64_LEVEL_2};
  table[49] = {-QAM64_LEVEL_2, -QAM64_LEVEL_1};
  table[50] = {-QAM64_LEVEL_1, -QAM64_LEVEL_2};
  table[51] = {-QAM64_LEVEL_1, -QAM64_LEVEL_1};
  table[52] = {-QAM64_LEVEL_2, -QAM64_LEVEL_3};
  table[53] = {-QAM64_LEVEL_2, -QAM64_LEVEL_4};
  table[54] = {-QAM64_LEVEL_1, -QAM64_LEVEL_3};
  table[55] = {-QAM64_LEVEL_1, -QAM64_LEVEL_4};
  table[56] = {-QAM64_LEVEL_3, -QAM64_LEVEL_2};
  table[57] = {-QAM64_LEVEL_3, -QAM64_LEVEL_1};
  table[58] = {-QAM64_LEVEL_4, -QAM64_LEVEL_2};
  table[59] = {-QAM64_LEVEL_4, -QAM64_LEVEL_1};
  table[60] = {-QAM64_LEVEL_3, -QAM64_LEVEL_3};
  table[61] = {-QAM64_LEVEL_3, -QAM64_LEVEL_4};
  table[62] = {-QAM64_LEVEL_4, -QAM64_LEVEL_3};
  table[63] = {-QAM64_LEVEL_4, -QAM64_LEVEL_4};
  Qm        = 6;
}

modulation_mapper_impl::qam256_table_s::qam256_table_s()
{
  // 256QAM constellation:
  // see [3GPP TS 38.211 version 15.8.0, Section 5.1.6]
  table.resize(256);
  for (uint32_t i = 0; i < 256; i++) {
    float offset = -1;
    float real   = 0;
    float imag   = 0;
    for (uint32_t j = 0; j < 4; j++) {
      real += offset;
      imag += offset;
      offset *= 2;

      real *= ((i & (1 << (2 * j + 1)))) ? +1 : -1;
      imag *= ((i & (1 << (2 * j + 0)))) ? +1 : -1;
    }
    table[i] = {real / sqrtf(170), imag / sqrtf(170)};
  }
  Qm = 8;
}

const std::map<modulation_scheme, modulation_mapper_impl::modulator_table_s> modulation_mapper_impl::modulation_tables =
    {
        {modulation_scheme::BPSK, modulation_mapper_impl::bpsk_table_s()},
        {modulation_scheme::QPSK, modulation_mapper_impl::qpsk_table_s()},
        {modulation_scheme::QAM16, modulation_mapper_impl::qam16_table_s()},
        {modulation_scheme::QAM64, modulation_mapper_impl::qam64_table_s()},
        {modulation_scheme::QAM256, modulation_mapper_impl::qam256_table_s()},
};

void modulation_mapper_impl::modulate(srsgnb::span<const uint8_t> input,
                                      srsgnb::span<srsgnb::cf_t>  symbols,
                                      srsgnb::modulation_scheme   scheme)
{
  const modulator_table_s& modulator = modulation_tables.at(scheme);
  const cf_t*              table     = modulator.table.data();
  unsigned                 Qm        = modulator.Qm;

  assert(input.size() == Qm * symbols.size());

  for (cf_t& symbol : symbols) {
    // Pack input bits
    unsigned idx = srsvec::bit_pack(input, Qm);

    // Assign symbol from table
    symbol = table[idx];
  }

  assert(input.size() == 0);
}

std::unique_ptr<modulation_mapper> srsgnb::create_modulation_mapper()
{
  return std::make_unique<modulation_mapper_impl>();
}
