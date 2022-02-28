///\file
///\brief ldpc_graph: class describing LDPC Tanner graphs.
#ifndef SRSGNB_UPPER_CHANNEL_CODING_LDPC_GRAPH_IMPL
#define SRSGNB_UPPER_CHANNEL_CODING_LDPC_GRAPH_IMPL

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
/// Maximum lifting size.
static constexpr unsigned max_lifting_size = 384;

/// \brief Representation of a graph, inspired by TS38.212 Tables 5.3.2-2 and 5.3.2-3.
///
/// Graphs are represented by a \c max_BG_M \f$\times\f$ \c max_BG_N_full matrix. A number between 0 and 383
/// at entry \f$(m, n)\f$ denotes a connection between check node \f$m\f$ and variable node \f$n\f$
/// in the base graph. The value of the number represents the circular shift applied to the connections
/// in the lifted graph. If entry \f$(m, n)\f$ is equal to \c NO_EDGE, there is no connection in either the
/// base graph or the lifted one.
using BG_matrix_t = std::array<std::array<uint16_t, max_BG_N_full>, max_BG_M>;

/// Maximum degree (number of incident edges) of a check node, in the base graphs.
static constexpr unsigned max_BG_check_edges = 20;
/// \brief Sparse representation of base graphs.
///
/// For check node \c m, BG_adjacency_matrix_t[m] provides a list of the variable nodes connected to it.
using BG_adjacency_matrix_t = std::array<std::array<uint16_t, max_BG_check_edges>, max_BG_M>;

/// Number of base graphs.
static constexpr unsigned nof_base_graphs = 2;
/// Total number of lifted graphs.
static constexpr unsigned total_nof_graphs = nof_lifting_sizes * nof_base_graphs;
} // namespace ldpc

/// Describes the Tanner graph of an LDPC code (implementation).
class ldpc_graph_impl
{
public:
  /// \name Constructors and destructors
  ///@{
  ldpc_graph_impl(ldpc::base_graph_t _bg, ldpc::lifting_size_t _ls);
  ldpc_graph_impl() : ldpc_graph_impl(ldpc::base_graph_t::BG1, ldpc::lifting_size_t::LS2) {}
  ~ldpc_graph_impl() = default;
  ///@}

  /// \name Copy constructor and operator
  /// Deleted
  ///@{
  ldpc_graph_impl(ldpc_graph_impl&) = delete;
  ldpc_graph_impl& operator=(ldpc_graph_impl&) = delete;
  ///@}
  /// \name Move constructor and operator
  /// Default
  ///@{
  ldpc_graph_impl(ldpc_graph_impl&&) = default;
  ldpc_graph_impl& operator=(ldpc_graph_impl&&) = default;
  ///@}

  /// Returns the base graph generating the current graph.
  ldpc::base_graph_t get_base_graph() const { return base_graph; }

  /// Returns the lifting size generating the current graph.
  ldpc::lifting_size_t get_lifting_size() const { return lifting_size; }

  /// Returns the lifting index corresponding to the current lifting size (a number between 0 and 7).
  uint8_t get_lifting_index() const { return lifting_index; }

  /// \brief Returns the compact representation of the lifted node at position \f$(m,n)\f$.
  ///
  /// If the base graph has an edge between check node \c m and variable node \c n, the returned value is
  /// the shift applied to the lifted node. Conversely, if there is no edge, then the returned value is
  /// srsgnb::ldpc::NO_EDGE.
  uint16_t get_lifted_node(unsigned m, unsigned n) const { return parity_check_matrix[m][n]; }

  /// Returns the number of check nodes of the underlying base graph.
  unsigned get_nof_BG_check_nodes() const { return nof_BG_check_nodes; }

  /// Returns the number of variable nodes of the underlying base graph.
  unsigned get_nof_BG_var_nodes_full() const { return nof_BG_var_nodes_full; }

  /// Returns the number of variable nodes of the underlying base graph after shortening.
  unsigned get_nof_BG_var_nodes_short() const { return nof_BG_var_nodes_short; }

  /// Returns the number of variable nodes of the underlying base graph associated to information bits.
  unsigned get_nof_BG_info_nodes() const { return nof_BG_var_nodes_full - nof_BG_check_nodes; }

private:
  /// Base graph.
  ldpc::base_graph_t base_graph;
  /// Lifting size.
  ldpc::lifting_size_t lifting_size;
  /// Lifting index.
  uint8_t lifting_index;
  /// Number of check nodes in the base graph.
  unsigned nof_BG_check_nodes;
  /// Number of variable nodes in the base graph, before shortening.
  unsigned nof_BG_var_nodes_full;
  /// Number of variable nodes in the base graph, after shortening.
  unsigned nof_BG_var_nodes_short;

  /// Parity check matrix of the graph (BG representation).
  ldpc::BG_matrix_t parity_check_matrix;
  /// Adjacency matrix of the base graph (sparse representation).
  const ldpc::BG_adjacency_matrix_t* adjacency_matrix;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_LDPC_IMPL
