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

#include "ldpc_graph_impl.h"
#include "ldpc_luts_impl.h"

using namespace srsran;
using namespace srsran::ldpc;

ldpc_graph_impl::ldpc_graph_impl(ldpc_base_graph_type _bg, ldpc::lifting_size_t _ls) :
  base_graph(_bg),
  lifting_size(_ls),
  lifting_index(::get_lifting_index(_ls)),
  parity_check_matrix(get_graph(_bg, _ls)),
  adjacency_matrix(get_adjacency_matrix(_bg))
{
  if (_bg == ldpc_base_graph_type::BG1) {
    nof_BG_check_nodes     = BG1_M;
    nof_BG_var_nodes_full  = BG1_N_FULL;
    nof_BG_var_nodes_short = BG1_N_SHORT;
  } else {
    nof_BG_check_nodes     = BG2_M;
    nof_BG_var_nodes_full  = BG2_N_FULL;
    nof_BG_var_nodes_short = BG2_N_SHORT;
  }

  for (unsigned k = 0, k_end = nof_BG_var_nodes_full - nof_BG_check_nodes; k != k_end; ++k) {
    for (unsigned m = 0; m != nof_BG_check_nodes; ++m) {
      if (parity_check_matrix[m][k] != NO_EDGE) {
        parity_check_sparse.push_back({m, k, parity_check_matrix[m][k]});
      }
    }
  }
}

std::array<ldpc_graph_impl, TOTAL_NOF_GRAPHS> srsran::ldpc::create_graph_array()
{
  std::array<ldpc_graph_impl, TOTAL_NOF_GRAPHS> tmp{};

  for (auto ls : ldpc::all_lifting_sizes) {
    uint8_t pos                  = get_lifting_size_position(ls);
    tmp[pos]                     = ldpc_graph_impl(ldpc_base_graph_type::BG1, ls);
    tmp[pos + NOF_LIFTING_SIZES] = ldpc_graph_impl(ldpc_base_graph_type::BG2, ls);
  }
  return tmp;
}
