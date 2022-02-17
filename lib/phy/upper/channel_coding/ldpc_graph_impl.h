///\file
///\brief ldpc_graph: class describing LDPC Tanner graphs.
#ifndef SRSGNB_CHANNEL_CODING_LDPC_GRAPH_IMPL
#define SRSGNB_CHANNEL_CODING_LDPC_GRAPH_IMPL

#include "srsgnb/phy/upper/channel_coding/ldpc.h"
#include <array>

namespace srsgnb {
namespace ldpc {

/// Placeholder for invalid lifting-size values.
static constexpr uint8_t VOID_LIFTSIZE = 255;
/// Placeholder for missing edges in the Tanner graph.
static constexpr uint16_t NO_EDGE = 0xFFFF;

/// Number of parity-check nodes in base graph BG1.
static constexpr unsigned BG1_M = 46;
/// Number of variable nodes in base graph BG1, before shortening.
static constexpr unsigned BG1_N_full = 68;
/// Number of variable nodes in base graph BG1, after shortening.
static constexpr unsigned BG1_N_short = 66;
/// Number of parity-check nodes in base graph BG2.
static constexpr unsigned BG2_M = 42;
/// Number of variable nodes in base graph BG2, before shortening.
static constexpr unsigned BG2_N_full = 52;
/// Number of variable nodes in base graph BG2, after shortening.
static constexpr unsigned BG2_N_short = 50;
/// Maximum number of variable nodes in a base graph.
static constexpr unsigned max_BG_N_full = std::max(BG1_N_full, BG2_N_full);
/// Maximum number of parity check nodes in a base graph.
static constexpr unsigned max_BG_M = std::max(BG1_M, BG2_M);

/// \brief Representation of a graph, inspired by TS38.212 Tables 5.3.2-2 and 5.3.2-3.
///
/// Graphs are represented by a \c max_BG_M \f$\times\f$ \c max_BG_N_full matrix. A number between 0 and 383
/// at entry \f$(m, n)\f$ denotes a connection between check node \f$m\f$ and variable node \f$n\f$
/// in the base graph. The value of the number represents the circular shift applied to the connections
/// in the lifted graph. If entry \f$(m, n)\f$ is equal to \c NO_EDGE, there is no connection in either the
/// base graph or the lifted one.
using BG_matrix_t = std::array<std::array<uint16_t, max_BG_N_full>, max_BG_M>;

/// Maximum degree (number of incident edges) of a check node, in the base graphs.
constexpr unsigned max_BG_check_edges = 20;
/// \brief Sparse representation of base graphs.
///
/// For check node \c m, BG_edge_matrix_t[m] provides a list of the variable nodes connected to it.
using BG_edge_matrix_t = std::array<std::array<uint16_t, max_BG_check_edges>, max_BG_M>;

} // namespace ldpc

/// Describes the Tanner graph of an LDPC code (implementation).
class ldpc_graph_impl
{
public:
  ldpc_graph_impl(ldpc::base_graph_t _bg, ldpc::lifting_size_t _ls);
  ldpc_graph_impl() : ldpc_graph_impl(ldpc::base_graph_t::BG1, ldpc::lifting_size_t::LS2){};
  ~ldpc_graph_impl() = default;

  // no move and copy constructors
  ldpc_graph_impl(ldpc_graph_impl&&) = delete;
  ldpc_graph_impl(ldpc_graph_impl&)  = delete;
  // no move and copy operators
  ldpc_graph_impl& operator=(ldpc_graph_impl&&) = delete;
  ldpc_graph_impl& operator=(ldpc_graph_impl&) = delete;

  uint8_t get_lifting_index();

private:
  /// Base graph.
  ldpc::base_graph_t base_graph;
  /// Lifting size.
  ldpc::lifting_size_t lifting_size;
  /// Number of check nodes in the base graph.
  unsigned nof_BG_check_nodes;
  /// Number of variable nodes in the base graph, before shortening.
  unsigned nof_BG_var_nodes_full;
  /// Number of variable nodes in the base graph, after shortening.
  unsigned nof_BG_var_nodes_short;

  /// Parity check matrix of the graph (BG representation).
  ldpc::BG_matrix_t parity_check_matrix;
  // int nof_check_nodes, nof_var_nodes_full, nof_var_nodes_short;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_LDPC_IMPL
