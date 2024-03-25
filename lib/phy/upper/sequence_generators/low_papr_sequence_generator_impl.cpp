/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "low_papr_sequence_generator_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/constants.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

// Sequence sizes used in PUCCH Format 1 and SRS.
const std::set<unsigned> low_papr_sequence_generator_impl::sequence_sizes = {
    12,  24,  36,  48,  60,  72,  84,  96,  108,  120,  132,  144,  156,  168,  180,  192,  204,  216,  228, 240,
    252, 264, 276, 288, 312, 324, 336, 360, 384,  396,  408,  432,  456,  480,  504,  528,  552,  576,  624, 648,
    672, 720, 768, 792, 816, 864, 912, 960, 1008, 1056, 1104, 1152, 1248, 1296, 1344, 1440, 1536, 1584, 1632};

const std::array<std::array<int, 6>, low_papr_sequence_generator_impl::NOF_ZC_SEQ>
    low_papr_sequence_generator_impl::phi_M_sc_6 = {
        {{-3, -1, 3, 3, -1, -3},  {-3, 3, -1, -1, 3, -3},  {-3, -3, -3, 3, 1, -3},  {1, 1, 1, 3, -1, -3},
         {1, 1, 1, -3, -1, 3},    {-3, 1, -1, -3, -3, -3}, {-3, 1, 3, -3, -3, -3},  {-3, -1, 1, -3, 1, -1},
         {-3, -1, -3, 1, -3, -3}, {-3, -3, 1, -3, 3, -3},  {-3, 1, 3, 1, -3, -3},   {-3, -1, -3, 1, 1, -3},
         {1, 1, 3, -1, -3, 3},    {1, 1, 3, 3, -1, 3},     {1, 1, 1, -3, 3, -1},    {1, 1, 1, -1, 3, -3},
         {-3, -1, -1, -1, 3, -1}, {-3, -3, -1, 1, -1, -3}, {-3, -3, -3, 1, -3, -1}, {-3, 1, 1, -3, -1, -3},
         {-3, 3, -3, 1, 1, -3},   {-3, 1, -3, -3, -3, -1}, {1, 1, -3, 3, 1, 3},     {1, 1, -3, -3, 1, -3},
         {1, 1, 3, -1, 3, 3},     {1, 1, -3, 1, 3, 3},     {1, 1, -1, -1, 3, -1},   {1, 1, -1, 3, -1, -1},
         {1, 1, -1, 3, -3, -1},   {1, 1, -3, 1, -1, -1}}};

const std::array<std::array<int, 12>, low_papr_sequence_generator_impl::NOF_ZC_SEQ>
    low_papr_sequence_generator_impl::phi_M_sc_12 = {
        {{-3, 1, -3, -3, -3, 3, -3, -1, 1, 1, 1, -3},  {-3, 3, 1, -3, 1, 3, -1, -1, 1, 3, 3, 3},
         {-3, 3, 3, 1, -3, 3, -1, 1, 3, -3, 3, -3},    {-3, -3, -1, 3, 3, 3, -3, 3, -3, 1, -1, -3},
         {-3, -1, -1, 1, 3, 1, 1, -1, 1, -1, -3, 1},   {-3, -3, 3, 1, -3, -3, -3, -1, 3, -1, 1, 3},
         {1, -1, 3, -1, -1, -1, -3, -1, 1, 1, 1, -3},  {-1, -3, 3, -1, -3, -3, -3, -1, 1, -1, 1, -3},
         {-3, -1, 3, 1, -3, -1, -3, 3, 1, 3, 3, 1},    {-3, -1, -1, -3, -3, -1, -3, 3, 1, 3, -1, -3},
         {-3, 3, -3, 3, 3, -3, -1, -1, 3, 3, 1, -3},   {-3, -1, -3, -1, -1, -3, 3, 3, -1, -1, 1, -3},
         {-3, -1, 3, -3, -3, -1, -3, 1, -1, -3, 3, 3}, {-3, 1, -1, -1, 3, 3, -3, -1, -1, -3, -1, -3},
         {1, 3, -3, 1, 3, 3, 3, 1, -1, 1, -1, 3},      {-3, 1, 3, -1, -1, -3, -3, -1, -1, 3, 1, -3},
         {-1, -1, -1, -1, 1, -3, -1, 3, 3, -1, -3, 1}, {-1, 1, 1, -1, 1, 3, 3, -1, -1, -3, 1, -3},
         {-3, 1, 3, 3, -1, -1, -3, 3, 3, -3, 3, -3},   {-3, -3, 3, -3, -1, 3, 3, 3, -1, -3, 1, -3},
         {3, 1, 3, 1, 3, -3, -1, 1, 3, 1, -1, -3},     {-3, 3, 1, 3, -3, 1, 1, 1, 1, 3, -3, 3},
         {-3, 3, 3, 3, -1, -3, -3, -1, -3, 1, 3, -3},  {3, -1, -3, 3, -3, -1, 3, 3, 3, -3, -1, -3},
         {-3, -1, 1, -3, 1, 3, 3, 3, -1, -3, 3, 3},    {-3, 3, 1, -1, 3, 3, -3, 1, -1, 1, -1, 1},
         {-1, 1, 3, -3, 1, -1, 1, -1, -1, -3, 1, -1},  {-3, -3, 3, 3, 3, -3, -1, 1, -3, 3, 1, -3},
         {1, -1, 3, 1, 1, -1, -1, -1, 1, 3, -3, 1},    {-3, 3, -3, 3, -3, -3, 3, -1, -1, 1, 3, -3}}};

const std::array<std::array<int, 18>, low_papr_sequence_generator_impl::NOF_ZC_SEQ>
    low_papr_sequence_generator_impl::phi_M_sc_18 = {
        {{-1, 3, -1, -3, 3, 1, -3, -1, 3, -3, -1, -1, 1, 1, 1, -1, -1, -1},
         {3, -3, 3, -1, 1, 3, -3, -1, -3, -3, -1, -3, 3, 1, -1, 3, -3, 3},
         {-3, 3, 1, -1, -1, 3, -3, -1, 1, 1, 1, 1, 1, -1, 3, -1, -3, -1},
         {-3, -3, 3, 3, 3, 1, -3, 1, 3, 3, 1, -3, -3, 3, -1, -3, -1, 1},
         {1, 1, -1, -1, -3, -1, 1, -3, -3, -3, 1, -3, -1, -1, 1, -1, 3, 1},
         {3, -3, 1, 1, 3, -1, 1, -1, -1, -3, 1, 1, -1, 3, 3, -3, 3, -1},
         {-3, 3, -1, 1, 3, 1, -3, -1, 1, 1, -3, 1, 3, 3, -1, -3, -3, -3},
         {1, 1, -3, 3, 3, 1, 3, -3, 3, -1, 1, 1, -1, 1, -3, -3, -1, 3},
         {-3, 1, -3, -3, 1, -3, -3, 3, 1, -3, -1, -3, -3, -3, -1, 1, 1, 3},
         {3, -1, 3, 1, -3, -3, -1, 1, -3, -3, 3, 3, 3, 1, 3, -3, 3, -3},
         {-3, -3, -3, 1, -3, 3, 1, 1, 3, -3, -3, 1, 3, -1, 3, -3, -3, 3},
         {-3, -3, 3, 3, 3, -1, -1, -3, -1, -1, -1, 3, 1, -3, -3, -1, 3, -1},
         {-3, -1, -3, -3, 1, 1, -1, -3, -1, -3, -1, -1, 3, 3, -1, 3, 1, 3},
         {1, 1, -3, -3, -3, -3, 1, 3, -3, 3, 3, 1, -3, -1, 3, -1, -3, 1},
         {-3, 3, -1, -3, -1, -3, 1, 1, -3, -3, -1, -1, 3, -3, 1, 3, 1, 1},
         {3, 1, -3, 1, -3, 3, 3, -1, -3, -3, -1, -3, -3, 3, -3, -1, 1, 3},
         {-3, -1, -3, -1, -3, 1, 3, -3, -1, 3, 3, 3, 1, -1, -3, 3, -1, -3},
         {-3, -1, 3, 3, -1, 3, -1, -3, -1, 1, -1, -3, -1, -1, -1, 3, 3, 1},
         {-3, 1, -3, -1, -1, 3, 1, -3, -3, -3, -1, -3, -3, 1, 1, 1, -1, -1},
         {3, 3, 3, -3, -1, -3, -1, 3, -1, 1, -1, -3, 1, -3, -3, -1, 3, 3},
         {-3, 1, 1, -3, 1, 1, 3, -3, -1, -3, -1, 3, -3, 3, -1, -1, -1, -3},
         {1, -3, -1, -3, 3, 3, -1, -3, 1, -3, -3, -1, -3, -1, 1, 3, 3, 3},
         {-3, -3, 1, -1, -1, 1, 1, -3, -1, 3, 3, 3, 3, -1, 3, 1, 3, 1},
         {3, -1, -3, 1, -3, -3, -3, 3, 3, -1, 1, -3, -1, 3, 1, 1, 3, 3},
         {3, -1, -1, 1, -3, -1, -3, -1, -3, -3, -1, -3, 1, 1, 1, -3, -3, 3},
         {-3, -3, 1, -3, 3, 3, 3, -1, 3, 1, 1, -3, -3, -3, 3, -3, -1, -1},
         {-3, -1, -1, -3, 1, -3, 3, -1, -1, -3, 3, 3, -3, -1, 3, -1, -1, -1},
         {-3, -3, 3, 3, -3, 1, 3, -1, -3, 1, -1, -3, 3, -3, -1, -1, -1, 3},
         {-1, -3, 1, -3, -3, -3, 1, 1, 3, 3, -3, 3, 3, -3, -1, 3, -3, 1},
         {-3, 3, 1, -1, -1, -1, -1, 1, -1, 3, 3, -3, -1, 1, 3, -1, 3, -1}}};

const std::array<std::array<int, 24>, low_papr_sequence_generator_impl::NOF_ZC_SEQ>
    low_papr_sequence_generator_impl::phi_M_sc_24 = {
        {{-1, -3, 3, -1, 3, 1, 3, -1, 1, -3, -1, -3, -1, 1, 3, -3, -1, -3, 3, 3, 3, -3, -3, -3},
         {-1, -3, 3, 1, 1, -3, 1, -3, -3, 1, -3, -1, -1, 3, -3, 3, 3, 3, -3, 1, 3, 3, -3, -3},
         {-1, -3, -3, 1, -1, -1, -3, 1, 3, -1, -3, -1, -1, -3, 1, 1, 3, 1, -3, -1, -1, 3, -3, -3},
         {1, -3, 3, -1, -3, -1, 3, 3, 1, -1, 1, 1, 3, -3, -1, -3, -3, -3, -1, 3, -3, -1, -3, -3},
         {-1, 3, -3, -3, -1, 3, -1, -1, 1, 3, 1, 3, -1, -1, -3, 1, 3, 1, -1, -3, 1, -1, -3, -3},
         {-3, -1, 1, -3, -3, 1, 1, -3, 3, -1, -1, -3, 1, 3, 1, -1, -3, -1, -3, 1, -3, -3, -3, -3},
         {-3, 3, 1, 3, -1, 1, -3, 1, -3, 1, -1, -3, -1, -3, -3, -3, -3, -1, -1, -1, 1, 1, -3, -3},
         {-3, 1, 3, -1, 1, -1, 3, -3, 3, -1, -3, -1, -3, 3, -1, -1, -1, -3, -1, -1, -3, 3, 3, -3},
         {-3, 1, -3, 3, -1, -1, -1, -3, 3, 1, -1, -3, -1, 1, 3, -1, 1, -1, 1, -3, -3, -3, -3, -3},
         {1, 1, -1, -3, -1, 1, 1, -3, 1, -1, 1, -3, 3, -3, -3, 3, -1, -3, 1, 3, -3, 1, -3, -3},
         {-3, -3, -3, -1, 3, -3, 3, 1, 3, 1, -3, -1, -1, -3, 1, 1, 3, 1, -1, -3, 3, 1, 3, -3},
         {-3, 3, -1, 3, 1, -1, -1, -1, 3, 3, 1, 1, 1, 3, 3, 1, -3, -3, -1, 1, -3, 1, 3, -3},
         {3, -3, 3, -1, -3, 1, 3, 1, -1, -1, -3, -1, 3, -3, 3, -1, -1, 3, 3, -3, -3, 3, -3, -3},
         {-3, 3, -1, 3, -1, 3, 3, 1, 1, -3, 1, 3, -3, 3, -3, -3, -1, 1, 3, -3, -1, -1, -3, -3},
         {-3, 1, -3, -1, -1, 3, 1, 3, -3, 1, -1, 3, 3, -1, -3, 3, -3, -1, -1, -3, -3, -3, 3, -3},
         {-3, -1, -1, -3, 1, -3, -3, -1, -1, 3, -1, 1, -1, 3, 1, -3, -1, 3, 1, 1, -1, -1, -3, -3},
         {-3, -3, 1, -1, 3, 3, -3, -1, 1, -1, -1, 1, 1, -1, -1, 3, -3, 1, -3, 1, -1, -1, -1, -3},
         {3, -1, 3, -1, 1, -3, 1, 1, -3, -3, 3, -3, -1, -1, -1, -1, -1, -3, -3, -1, 1, 1, -3, -3},
         {-3, 1, -3, 1, -3, -3, 1, -3, 1, -3, -3, -3, -3, -3, 1, -3, -3, 1, 1, -3, 1, 1, -3, -3},
         {-3, -3, 3, 3, 1, -1, -1, -1, 1, -3, -1, 1, -1, 3, -3, -1, -3, -1, -1, 1, -3, 3, -1, -3},
         {-3, -3, -1, -1, -1, -3, 1, -1, -3, -1, 3, -3, 1, -3, 3, -3, 3, 3, 1, -1, -1, 1, -3, -3},
         {3, -1, 1, -1, 3, -3, 1, 1, 3, -1, -3, 3, 1, -3, 3, -1, -1, -1, -1, 1, -3, -3, -3, -3},
         {-3, 1, -3, 3, -3, 1, -3, 3, 1, -1, -3, -1, -3, -3, -3, -3, 1, 3, -1, 1, 3, 3, 3, -3},
         {-3, -1, 1, -3, -1, -1, 1, 1, 1, 3, 3, -1, 1, -1, 1, -1, -1, -3, -3, -3, 3, 1, -1, -3},
         {-3, 3, -1, -3, -1, -1, -1, 3, -1, -1, 3, -3, -1, 3, -3, 3, -3, -1, 3, 1, 1, -1, -3, -3},
         {-3, 1, -1, -3, -3, -1, 1, -3, -1, -3, 1, 1, -1, 1, 1, 3, 3, 3, -1, 1, -1, 1, -1, -3},
         {-1, 3, -1, -1, 3, 3, -1, -1, -1, 3, -1, -3, 1, 3, 1, 1, -3, -3, -3, -1, -3, -1, -3, -3},
         {3, -3, -3, -1, 3, 3, -3, -1, 3, 1, 1, 1, 3, -1, 3, -3, -1, 3, -1, 3, 1, -1, -3, -3},
         {-3, 1, -3, 1, -3, 1, 1, 3, 1, -3, -3, -1, 1, 3, -1, -3, 3, 1, -1, -3, -3, -3, -3, -3},
         {3, -3, -1, 1, 3, -1, -1, -3, -1, 3, -1, -3, -1, -3, 3, -1, 3, 1, 1, -3, 3, -3, -3, -3}}};

static int zc_sequence_q(uint32_t u, uint32_t v, uint32_t N_sz)
{
  float q;
  float q_hat;
  float n_sz = (float)N_sz;

  q_hat = n_sz * (u + 1) / 31;
  if ((((uint32_t)(2 * q_hat)) % 2) == 0) {
    q = q_hat + 0.5 + v;
  } else {
    q = q_hat + 0.5 - v;
  }
  return static_cast<int>(q);
}

static unsigned get_N_zc(unsigned M_zc)
{
  if (M_zc >= 36) {
    return prime_lower_than(M_zc);
  }

  return 4;
}

span<const int> low_papr_sequence_generator_impl::r_uv_arg_mprb(unsigned u, unsigned v, unsigned M_zc)
{
  // Select temporary argument.
  srsran_assert(M_zc <= temp_r_uv_arg.size(),
                "Sequence length (i.e., {}) exceeds maximum sequence size (i.e., {})",
                M_zc,
                temp_r_uv_arg.size());
  span<int> r_uv_arg = span<int>(temp_r_uv_arg).first(M_zc);
  unsigned  N_zc     = get_N_zc(M_zc);
  int64_t   q        = zc_sequence_q(u, v, N_zc);
  for (unsigned n = 0; n != M_zc; ++n) {
    int64_t m   = (n % N_zc);
    r_uv_arg[n] = -static_cast<int>((q * m * (m + 1)) % (2 * N_zc));
  }

  return r_uv_arg;
}

span<const int> low_papr_sequence_generator_impl::r_uv_arg(unsigned u, unsigned v, unsigned M_zc)
{
  if (M_zc == 6) {
    return phi_M_sc_6[u];
  } else if (M_zc == 12) {
    return phi_M_sc_12[u];
  } else if (M_zc == 18) {
    return phi_M_sc_18[u];
  } else if (M_zc == 24) {
    return phi_M_sc_24[u];
  } else if (M_zc >= 36) {
    return r_uv_arg_mprb(u, v, M_zc);
  }

  srsran_terminate("Invalid sequence length {}", M_zc);
  return {};
}

low_papr_sequence_generator_impl::low_papr_sequence_generator_impl() : cs_table(24, 1)
{
  unsigned max_size = 0;
  for (unsigned M_zc : sequence_sizes) {
    // Select number of elements in the complex exponential.
    unsigned table_size = 2 * get_N_zc(M_zc);

    // Create complex exponential table if it has not been created earlier.
    if (tables.count(table_size) == 0) {
      tables.emplace(table_size, complex_exponential_table(table_size, 1.0));

      max_size = std::max(max_size, M_zc);
    }
  }

  temp_r_uv_arg.resize(max_size);
  temp_cyclic_shift.resize(max_size);
}

void low_papr_sequence_generator_impl::generate(span<cf_t> sequence,
                                                unsigned   u,
                                                unsigned   v,
                                                unsigned   alpha_num,
                                                unsigned   alpha_den)
{
  // Select number of elements in the complex exponential.
  unsigned table_size = 2 * get_N_zc(sequence.size());
  srsran_assert(tables.count(table_size), "Sequence generator was not initialized with table size {}", table_size);

  // Select complex exponential table.
  complex_exponential_table& table = tables.at(table_size);

  // Verify the cyclic shift is valid.
  srsran_assert((alpha_num == 0) || (cs_table.size() % alpha_den == 0),
                "The cyclic shift denominator (i.e., {}) is not compatible with a table size of (i.e., {})",
                alpha_den,
                cs_table.size());

  // Calculate argument.
  span<const int> arg = r_uv_arg(u, v, sequence.size());

  // Generate sequence from the argument.
  table.generate(sequence, arg);

  // Apply cyclic shift.
  if (alpha_num != 0) {
    span<cf_t> cyclic_shift = span<cf_t>(temp_cyclic_shift).first(sequence.size());
    cs_table.generate(cyclic_shift, 0, alpha_num * cs_table.size() / alpha_den);
    srsvec::prod(sequence, cyclic_shift, sequence);
  }
}
