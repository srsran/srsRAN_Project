/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
