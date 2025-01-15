/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../../lib/ofh/transmitter/sequence_identifier_generator.h"
#include "srsran/adt/bounded_bitset.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

TEST(sequence_identifier_generator, generate_consecutive_sequence_number_for_one_exac)
{
  unsigned eaxc = 0;

  sequence_identifier_generator gen;

  unsigned seq_id = gen.generate(eaxc);
  ASSERT_EQ(++seq_id, gen.generate(eaxc));
}

TEST(sequence_identifier_generator, generate_consecutive_sequence_number_for_different_exac)
{
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> eaxc = {0, 1, 2, 3};

  sequence_identifier_generator gen;

  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> seq_id = {0, 0, 0, 0};
  for (unsigned i = 0; i != MAX_NOF_SUPPORTED_EAXC; ++i) {
    for (unsigned eaxc_pos = 0, eaxc_end = eaxc.size(); eaxc_pos != eaxc_end; ++eaxc_pos) {
      if (i == 0) {
        seq_id[eaxc_pos] = gen.generate(eaxc[eaxc_pos]);
        continue;
      }

      ASSERT_EQ(++seq_id[eaxc_pos], gen.generate(eaxc[eaxc_pos]));
    }
  }
}

TEST(sequence_identifier_generator, sequence_id_values_fit_in_one_byte)
{
  unsigned eaxc = 0;

  sequence_identifier_generator gen(255);

  ASSERT_EQ(gen.generate(eaxc), 255);
  ASSERT_EQ(gen.generate(eaxc), 0);
}

#ifdef ASSERTS_ENABLED
TEST(sequence_identifier_generator, death_when_eaxc_value_is_not_supported)
{
  unsigned eaxc = MAX_SUPPORTED_EAXC_ID_VALUE;

  sequence_identifier_generator gen;
  ASSERT_DEATH(gen.generate(eaxc), "");
}
#endif
