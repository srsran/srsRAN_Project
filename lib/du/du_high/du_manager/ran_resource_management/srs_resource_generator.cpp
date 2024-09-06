/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srs_resource_generator.h"

using namespace srsran;
using namespace srs_du;

std::vector<du_srs_resource> srsran::srs_du::generate_cell_srs_list(const du_cell_config& du_cell_cfg)
{
  std::vector<du_srs_resource> srs_res_list;
  // Compute the available TX comb offsets. \ref tx_comb_cyclic_shift, in \c srs_config::srs_resource::tx_comb_params.
  std::vector<unsigned> tx_comb_offsets = du_cell_cfg.srs_cfg.tx_comb == srsran::tx_comb_size::n2
                                              ? std::vector<unsigned>{0U, 1U}
                                              : std::vector<unsigned>{0U, 1U, 2U, 3U};

  // Compute the available Cyclic Shifts.
  const unsigned        max_cs  = du_cell_cfg.srs_cfg.tx_comb == srsran::tx_comb_size::n2 ? 8U : 12U;
  const unsigned        cs_step = max_cs / static_cast<unsigned>(du_cell_cfg.srs_cfg.cyclic_shift_reuse_factor);
  std::vector<unsigned> cs_values;
  for (unsigned cs = 0; cs < max_cs; cs += cs_step) {
    cs_values.push_back(cs);
  }

  // Compute the available Sequence IDs.
  // NOTE: we only consider the number of orthogonal sequences that can be generated, as per TS 38.211,
  // Section 6.4.1.4.2, which is 30.
  constexpr unsigned max_seq_id_values = 30U;
  const unsigned seq_id_step = max_seq_id_values / static_cast<unsigned>(du_cell_cfg.srs_cfg.sequence_id_reuse_factor);
  std::vector<unsigned> seq_id_values;
  for (unsigned seq_id = 0; seq_id < max_seq_id_values; seq_id += seq_id_step) {
    seq_id_values.push_back(static_cast<unsigned>(du_cell_cfg.pci) + seq_id);
  }

  // Find the first symbol within the UL slot (considering all options FDD, TDD pattern 1 and TDD pattern 2) where the
  // SRS resource can be placed.
  unsigned starting_sym = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - du_cell_cfg.srs_cfg.max_nof_symbols.to_uint();
  if (du_cell_cfg.tdd_ul_dl_cfg_common.has_value()) {
    const auto& tdd_cfg = du_cell_cfg.tdd_ul_dl_cfg_common.value();
    starting_sym        = std::min(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - tdd_cfg.pattern1.nof_ul_symbols);
    if (tdd_cfg.pattern2.has_value()) {
      starting_sym = std::min(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - tdd_cfg.pattern2.value().nof_ul_symbols);
    }
  }
  // The number of SRS symbols cannot be larger than 6.
  starting_sym = std::max(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - du_cell_cfg.srs_cfg.max_nof_symbols.max());

  // We use the counter to define the cell resource ID.
  unsigned srs_res_cnt = 0;
  for (unsigned sym_start = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - static_cast<unsigned>(du_cell_cfg.srs_cfg.nof_symbols);
       sym_start >= starting_sym;
       sym_start -= static_cast<unsigned>(du_cell_cfg.srs_cfg.nof_symbols)) {
    const ofdm_symbol_range srs_res_symbols{sym_start,
                                            sym_start + static_cast<unsigned>(du_cell_cfg.srs_cfg.nof_symbols)};
    for (auto tx_comb_offset : tx_comb_offsets) {
      for (auto cs : cs_values) {
        for (auto seq_id : seq_id_values) {
          du_srs_resource srs_res;
          srs_res.cell_res_id    = srs_res_cnt;
          srs_res.tx_comb_offset = tx_comb_offset;
          srs_res.symbols        = srs_res_symbols;
          srs_res.sequence_id    = seq_id;
          srs_res.cs             = cs;
          srs_res_list.push_back(srs_res);
          ++srs_res_cnt;
        }
      }
    }
  }
  return srs_res_list;
}
