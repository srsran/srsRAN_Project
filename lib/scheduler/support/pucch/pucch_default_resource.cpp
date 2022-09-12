/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_default_resource.h"

using namespace srsgnb;

unsigned srsgnb::get_pucch_default_resource_index(unsigned int n_cce, unsigned int nof_cce, unsigned int delta_pri)
{
  return (2 * n_cce) / nof_cce + 2 * delta_pri;
}

std::pair<unsigned, unsigned>
srsgnb::get_pucch_default_prb_index(unsigned r_pucch, unsigned rb_bwp_offset, unsigned nof_cs, unsigned N_bwp_size)
{
  srsgnb_assert(r_pucch < 16, "The PUCCH resource index {} exceeds the maximum allowed {}.", r_pucch, 16);

  unsigned prb_index_first  = rb_bwp_offset + (r_pucch / nof_cs);
  unsigned prb_index_second = N_bwp_size - 1 - prb_index_first;
  if (r_pucch / 8 == 1) {
    prb_index_second = rb_bwp_offset + ((r_pucch - 8) / nof_cs);
    prb_index_first  = N_bwp_size - 1 - prb_index_second;
  }

  return {prb_index_first, prb_index_second};
}

unsigned srsgnb::get_pucch_default_cyclic_shift(unsigned r_pucch, unsigned nof_cs)
{
  srsgnb_assert(r_pucch < 16, "The PUCCH resource index {} exceeds the maximum allowed {}.", r_pucch, 16);
  srsgnb_assert(nof_cs > 0, "The number of cyclic shift must be greater than zero.");
  unsigned result = r_pucch % nof_cs;
  if (r_pucch > 8) {
    result = (r_pucch - 8) % nof_cs;
  }
  return result;
}

pucch_default_resource srsgnb::get_pucch_default_resource(unsigned index, unsigned N_bwp_size)
{
  // TS38.213 Table 9.2.1-1.
  static const std::array<pucch_default_resource, 16> table = {{{pucch_format::FORMAT_0, 12, 2, 0, {0, 3}},
                                                                {pucch_format::FORMAT_0, 12, 2, 0, {0, 4, 8}},
                                                                {pucch_format::FORMAT_0, 12, 2, 3, {0, 4, 8}},
                                                                {pucch_format::FORMAT_1, 10, 4, 0, {0, 6}},
                                                                {pucch_format::FORMAT_1, 10, 4, 0, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 10, 4, 2, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 10, 4, 4, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 4, 10, 0, {0, 6}},
                                                                {pucch_format::FORMAT_1, 4, 10, 0, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 4, 10, 2, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 4, 10, 4, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 0, 14, 0, {0, 6}},
                                                                {pucch_format::FORMAT_1, 0, 14, 0, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 0, 14, 2, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 0, 14, 4, {0, 3, 6, 9}},
                                                                {pucch_format::FORMAT_1, 0, 14, 0, {0, 3, 6, 9}}}};

  srsgnb_assert(
      index < table.size(), "PUCCH resource index {} exceeds the number of elements {}.", index, table.size());
  pucch_default_resource result = table[index];

  // Handle PRB offset for index 15.
  if (index == 15) {
    result.rb_bwp_offset = N_bwp_size / 4;
  }

  return result;
}