#include "ldpc_graph_impl.h"
#include "ldpc_luts_impl.h"
#include <cassert>

using namespace srsgnb;
using namespace srsgnb::ldpc;

ldpc_graph_impl::ldpc_graph_impl(ldpc::base_graph_t _bg, ldpc::lifting_size_t _ls) :
  base_graph(_bg), lifting_size(_ls), parity_check_matrix(get_graph(_bg, _ls))
{
  assert(_bg == ldpc::base_graph_t::BG1); // BG2 not implemented yet
  assert(_ls < 5);                        // other ls not implemented yet

  nof_BG_check_nodes     = BG1_M;
  nof_BG_var_nodes_full  = BG1_N_full;
  nof_BG_var_nodes_short = BG1_N_short;
}

uint8_t srsgnb::ldpc_graph_impl::get_lifting_index()
{
  return ::get_lifting_index(lifting_size);
}