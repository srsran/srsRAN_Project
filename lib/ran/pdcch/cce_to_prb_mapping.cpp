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

#include "srsran/ran/pdcch/cce_to_prb_mapping.h"

using namespace srsran;

// Describes a REG index list of unsigned 16 bit indicating the REG index.
using reg_index_list = static_vector<uint16_t, pdcch_constants::MAX_NOF_RB_PDCCH>;

static reg_index_list cce_to_reg_mapping_non_interleaved(unsigned aggregation_level, unsigned cce_index)
{
  reg_index_list result;

  unsigned reg_index_begin = pdcch_constants::NOF_REG_PER_CCE * cce_index;
  unsigned reg_index_end   = reg_index_begin + pdcch_constants::NOF_REG_PER_CCE * aggregation_level;
  for (unsigned reg_index = reg_index_begin; reg_index != reg_index_end; ++reg_index) {
    result.push_back(reg_index);
  }

  return result;
}

static reg_index_list cce_to_reg_mapping_interleaved(unsigned N_rb_coreset,
                                                     unsigned N_symb_coreset,
                                                     unsigned reg_bundle_size,
                                                     unsigned interleaver_size,
                                                     unsigned shift_index,
                                                     unsigned aggregation_level,
                                                     unsigned cce_index)
{
  reg_index_list result;

  // Load interleave parameters.
  unsigned L             = reg_bundle_size;
  unsigned R             = interleaver_size;
  unsigned N_reg_coreset = N_rb_coreset * N_symb_coreset;
  unsigned C             = N_reg_coreset / (L * R);
  unsigned n_shift       = shift_index;

  // Validate parameters.
  srsran_assert(N_reg_coreset > 0 && (N_reg_coreset % (L * R) == 0) && (L % N_symb_coreset == 0),
                "Invalid CORESET configuration N_reg_coreset={}, al={}, R={}, Nsymb_coreset={}.",
                N_reg_coreset,
                L,
                R,
                N_symb_coreset);

  // Number of REG bundles in a CCE.
  unsigned nof_reg_bundle_in_cce = pdcch_constants::NOF_REG_PER_CCE / L;

  // Calculate the REG bundle boundaries for the PDCCH transmission.
  unsigned reg_bundle_index_begin = cce_index * nof_reg_bundle_in_cce;
  unsigned reg_bundle_index_end   = (cce_index + aggregation_level) * nof_reg_bundle_in_cce;

  // For each REG bundle index in the CCE...
  for (unsigned reg_bundle_index = reg_bundle_index_begin; reg_bundle_index != reg_bundle_index_end;
       ++reg_bundle_index) {
    // Deduce r and c from x = c * R + r, where x is the source REG bundle index.
    unsigned r = reg_bundle_index % R;
    unsigned c = reg_bundle_index / R;

    // Calculate destination REG bundle index f(x) = (r * C + c + n_shift) % (N_reg_coreset / L).
    unsigned dst_reg_bundle_index = (r * C + c + n_shift) % (N_reg_coreset / L);

    // For each REG in the REG bundle.
    unsigned reg_index_start = dst_reg_bundle_index * L;
    unsigned reg_index_end   = (dst_reg_bundle_index + 1) * L;
    for (unsigned reg_index = reg_index_start; reg_index != reg_index_end; ++reg_index) {
      result.push_back(reg_index);
    }
  }

  // Reorder result.
  std::sort(result.begin(), result.end());

  return result;
}

static prb_index_list reg_to_prb_mapping_coreset0(unsigned              N_coreset0_start,
                                                  unsigned              N_coreset0_size,
                                                  unsigned              N_symb_coreset,
                                                  const reg_index_list& reg_indexes)
{
  prb_index_list result;

  for (unsigned i = 0; i != reg_indexes.size(); i += N_symb_coreset) {
    unsigned prb_index = reg_indexes[i] / N_symb_coreset + N_coreset0_start;
    result.push_back(prb_index);
  }

  return result;
}

static prb_index_list reg_to_prb_mapping_other(unsigned              N_bwp_start,
                                               freq_resource_bitmap  freq_resources,
                                               unsigned              N_symb_coreset,
                                               const reg_index_list& reg_indexes)
{
  prb_index_list result;
  unsigned       reg_count = 0;
  unsigned       reg_index = 0;

  // For each frequency resource...
  int freq_resource_index = freq_resources.find_lowest(0, freq_resources.size());
  while (freq_resource_index != -1) {
    // Calculate PRB ranges for the frequency resource
    unsigned prb_index_begin = freq_resource_index * 6 + N_bwp_start;
    unsigned prb_index_end   = prb_index_begin + 6;

    // Iterate all the resource blocks in the resource.
    for (unsigned prb_index = prb_index_begin; prb_index != prb_index_end; ++prb_index, reg_index += N_symb_coreset) {
      // Skip if the REG index does not match with the next REG.
      if (reg_index != reg_indexes[reg_count]) {
        continue;
      }

      // Add the current PRB index and advance the REG counter.
      result.push_back(prb_index);
      reg_count += N_symb_coreset;

      if (reg_count == reg_indexes.size()) {
        return result;
      }
    }

    freq_resource_index = freq_resources.find_lowest(freq_resource_index + 1, freq_resources.size());
  }

  return result;
}

prb_index_list srsran::cce_to_prb_mapping_coreset0(unsigned N_coreset0_start,
                                                   unsigned N_coreset0_size,
                                                   unsigned N_symb_coreset,
                                                   pci_t    N_id_cell,
                                                   unsigned aggregation_level,
                                                   unsigned cce_index)
{
  // Calculate the REG indexes.
  reg_index_list reg_indexes =
      cce_to_reg_mapping_interleaved(N_coreset0_size, N_symb_coreset, 6, 2, N_id_cell, aggregation_level, cce_index);

  return reg_to_prb_mapping_coreset0(N_coreset0_start, N_coreset0_size, N_symb_coreset, reg_indexes);
}

prb_index_list srsran::cce_to_prb_mapping_non_interleaved(unsigned                    N_bwp_start,
                                                          const freq_resource_bitmap& freq_resources,
                                                          unsigned                    N_symb_coreset,
                                                          unsigned                    aggregation_level,
                                                          unsigned                    cce_index)
{
  // Calculate the REG indexes.
  reg_index_list reg_indexes = cce_to_reg_mapping_non_interleaved(aggregation_level, cce_index);

  // Map REG indexes to PRB.
  return reg_to_prb_mapping_other(N_bwp_start, freq_resources, N_symb_coreset, reg_indexes);
}

prb_index_list srsran::cce_to_prb_mapping_interleaved(unsigned                    N_bwp_start,
                                                      const freq_resource_bitmap& freq_resources,
                                                      unsigned                    N_symb_coreset,
                                                      unsigned                    reg_bundle_size,
                                                      unsigned                    interleaver_size,
                                                      unsigned                    shift_index,
                                                      unsigned                    aggregation_level,
                                                      unsigned                    cce_index)
{
  // Calculate the REG indexes.
  reg_index_list reg_indexes = cce_to_reg_mapping_interleaved(freq_resources.count() * 6,
                                                              N_symb_coreset,
                                                              reg_bundle_size,
                                                              interleaver_size,
                                                              shift_index,
                                                              aggregation_level,
                                                              cce_index);

  // Map REG indexes to PRB.
  return reg_to_prb_mapping_other(N_bwp_start, freq_resources, N_symb_coreset, reg_indexes);
}
