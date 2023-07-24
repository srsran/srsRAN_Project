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

///\file
///\brief ldpc_graph: class describing LDPC Tanner graphs.
#pragma once

#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include <array>

namespace srsran {
namespace ldpc {

/// Placeholder for invalid lifting-size values.
static constexpr uint8_t VOID_LIFTSIZE = 255;
/// Placeholder for missing edges in the Tanner graph.
static constexpr uint16_t NO_EDGE = 0xffff;

/// Number of parity-check nodes in base graph BG1.
static constexpr unsigned BG1_M = 46;
/// Number of variable nodes in base graph BG1, before shortening.
static constexpr unsigned BG1_N_FULL = 68;
/// Number of variable nodes in base graph BG1, after shortening.
static constexpr unsigned BG1_N_SHORT = 66;
/// Number of parity-check nodes in base graph BG2.
static constexpr unsigned BG2_M = 42;
/// Number of variable nodes in base graph BG2, before shortening.
static constexpr unsigned BG2_N_FULL = 52;
/// Number of variable nodes in base graph BG2, after shortening.
static constexpr unsigned BG2_N_SHORT = 50;
/// Maximum number of variable nodes in a base graph.
static constexpr unsigned MAX_BG_N_FULL = std::max(BG1_N_FULL, BG2_N_FULL);
/// Maximum number of parity check nodes in a base graph.
static constexpr unsigned MAX_BG_M = std::max(BG1_M, BG2_M);
/// Maximum lifting size.
static constexpr unsigned MAX_LIFTING_SIZE = 384;

/// \brief Representation of a graph, inspired by TS38.212 Tables 5.3.2-2 and 5.3.2-3.
///
/// Graphs are represented by a \c max_BG_M \f$\times\f$ \c max_BG_N_full matrix. A number between 0 and 383
/// at entry \f$(m, n)\f$ denotes a connection between check node \f$m\f$ and variable node \f$n\f$
/// in the base graph. The value of the number represents the circular shift applied to the connections
/// in the lifted graph. If entry \f$(m, n)\f$ is equal to \c NO_EDGE, there is no connection in either the
/// base graph or the lifted one.
using BG_matrix_t = std::array<std::array<uint16_t, MAX_BG_N_FULL>, MAX_BG_M>;

/// Maximum degree (number of incident edges) of a check node, in the base graphs.
static constexpr unsigned MAX_BG_CHECK_EDGES = 20;

/// \brief Represents a list of variable nodes connected to a check node.
using BG_adjacency_row_t = std::array<uint16_t, MAX_BG_CHECK_EDGES>;

/// \brief For check node \c m, BG_adjacency_matrix_t[m] provides a list of the variable nodes connected to it.
using BG_adjacency_matrix_t = std::array<BG_adjacency_row_t, MAX_BG_M>;

/// Number of base graphs.
static constexpr unsigned NOF_BASE_GRAPHS = 2;
/// Total number of lifted graphs.
static constexpr unsigned TOTAL_NOF_GRAPHS = NOF_LIFTING_SIZES * NOF_BASE_GRAPHS;
} // namespace ldpc

/// Describes the Tanner graph of an LDPC code (implementation).
class ldpc_graph_impl
{
public:
  /// \name Constructors and destructors

  ///@{
  ldpc_graph_impl(ldpc_base_graph_type _bg, ldpc::lifting_size_t _ls);
  ldpc_graph_impl() : ldpc_graph_impl(ldpc_base_graph_type::BG1, ldpc::lifting_size_t::LS2) {}
  ~ldpc_graph_impl() = default;
  ///@}

  /// \name Copy constructor and operator
  /// Deleted

  ///@{
  ldpc_graph_impl(ldpc_graph_impl&)            = delete;
  ldpc_graph_impl& operator=(ldpc_graph_impl&) = delete;
  ///@}

  /// \name Move constructor and operator
  /// Default

  ///@{
  ldpc_graph_impl(ldpc_graph_impl&&)            = default;
  ldpc_graph_impl& operator=(ldpc_graph_impl&&) = default;
  ///@}

  /// Returns the base graph generating the current graph.
  ldpc_base_graph_type get_base_graph() const { return base_graph; }

  /// Returns the lifting size generating the current graph.
  ldpc::lifting_size_t get_lifting_size() const { return lifting_size; }

  /// Returns the lifting index corresponding to the current lifting size (a number between 0 and 7).
  uint8_t get_lifting_index() const { return lifting_index; }

  /// \brief Returns the compact representation of the lifted node at position \f$(m,n)\f$.
  ///
  /// If the base graph has an edge between check node \c m and variable node \c n, the returned value is
  /// the shift applied to the lifted node. Conversely, if there is no edge, then the returned value is
  /// srsran::ldpc::NO_EDGE.
  uint16_t get_lifted_node(unsigned m, unsigned n) const { return parity_check_matrix[m][n]; }

  /// Returns the number of check nodes of the underlying base graph.
  unsigned get_nof_BG_check_nodes() const { return nof_BG_check_nodes; }

  /// Returns the number of variable nodes of the underlying base graph.
  unsigned get_nof_BG_var_nodes_full() const { return nof_BG_var_nodes_full; }

  /// Returns the number of variable nodes of the underlying base graph after shortening.
  unsigned get_nof_BG_var_nodes_short() const { return nof_BG_var_nodes_short; }

  /// Returns the number of variable nodes of the underlying base graph associated to information bits.
  unsigned get_nof_BG_info_nodes() const { return nof_BG_var_nodes_full - nof_BG_check_nodes; }

  /// Returns a reference to the selected row of ldpc_graph_impl::adjacency_matrix.
  const ldpc::BG_adjacency_row_t& get_adjacency_row(unsigned m) const { return (*adjacency_matrix)[m]; }

  const std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>& get_parity_check_sparse() const
  {
    return parity_check_sparse;
  }

private:
  /// Base graph.
  ldpc_base_graph_type base_graph;
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

  std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> parity_check_sparse;

  /// Adjacency matrix of the base graph (sparse representation).
  const ldpc::BG_adjacency_matrix_t* adjacency_matrix;
};

namespace ldpc {
/// \brief Creates an array with all possible LDPC graphs.
///
/// The graphs will be ordered by base graph and lifting size. For instance, the graph corresponding to BG2 and LS4 will
/// be at entry ::NOF_LIFTING_SIZES <tt> + 3 </tt>, since \c LS4 is the third one in the list of all possible lifting
/// sizes (i.e., ::all_lifting_sizes).
std::array<ldpc_graph_impl, TOTAL_NOF_GRAPHS> create_graph_array();

/// An array with all possible LDPC graphs (see create_graph_array()).
static const std::array<ldpc_graph_impl, TOTAL_NOF_GRAPHS> graph_array = create_graph_array();
} // namespace ldpc

} // namespace srsran
