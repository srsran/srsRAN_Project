#include "ldpc_graph_impl.h"
#include "ldpc_luts_impl.h"
#include <cassert>

using namespace srsgnb;
using namespace srsgnb::ldpc;

ldpc_graph_impl::ldpc_graph_impl(ldpc::base_graph_t _bg, ldpc::lifting_size_t _ls) :
  base_graph(_bg),
  lifting_size(_ls),
  lifting_index(::get_lifting_index(_ls)),
  parity_check_matrix(get_graph(_bg, _ls)),
  adjacency_matrix(get_adjacency_matrix(_bg))
{
  if (_bg == base_graph_t::BG1) {
    nof_BG_check_nodes     = BG1_M;
    nof_BG_var_nodes_full  = BG1_N_full;
    nof_BG_var_nodes_short = BG1_N_short;
  } else {
    nof_BG_check_nodes     = BG2_M;
    nof_BG_var_nodes_full  = BG2_N_full;
    nof_BG_var_nodes_short = BG2_N_short;
  }
}