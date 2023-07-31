/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "resource_grid_test_doubles.h"

namespace srsran {

/// Resource grid mapper spy.
class resource_grid_mapper_spy : public resource_grid_mapper
{
public:
  /// Constructs a resource grid mapper that writes using a resource grid writer spy.
  explicit resource_grid_mapper_spy(resource_grid_writer_spy& grid_writer_spy) : rg_writer_spy(grid_writer_spy)
  {
    // Do nothing.
  }

  /// \brief Maps the input resource elements using a resource grid writer spy.
  ///
  /// The mapping into the resource grid does not use precoding, therefore, the resource grid is filled with the symbols
  /// corresponding to each layer.
  ///
  /// \param[in] input input resource elements arranged by layer.
  /// \param[in] pattern RE pattern representing the grid allocation.
  /// \param[in] reserved Reserved resource elements, to be excluded from the allocation pattern.
  void map(const re_buffer_reader& input,
           const re_pattern_list&  pattern,
           const re_pattern_list&  reserved,
           const precoding_configuration& /* precoding */) override;

  /// \brief TBD
  void map(unsigned                            i_symbol,
           unsigned                            i_subcarrier,
           const bounded_bitset<NRE * MAX_RB>& mask,
           const precoding_weight_matrix&      precoding,
           span<const cf_t>                    symbols) override;

  /// \brief Maps the input resource elements using a resource grid writer spy.
  ///
  /// The mapping into the resource grid does not use precoding, therefore, the resource grid is filled with the symbols
  /// corresponding to each layer.
  ///
  /// \param[in] input input resource elements arranged by layer.
  /// \param[in] pattern RE pattern representing the grid allocation.
  void map(const re_buffer_reader& input,
           const re_pattern_list&  pattern,
           const precoding_configuration& /* precoding */) override;

  resource_grid_writer_spy& rg_writer_spy;
};

} // namespace srsran
