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

  /// \brief Maps complex symbols onto the resource grid.
  ///
  /// The resource mapping comprises layer mapping and copying onto the resource grid.
  ///
  /// The mapping is performed over the symbol index \c i_symbol, starting at subcarrier index \c i_subcarrier and for
  /// the subcarriers indicated by \c mask. The number of layers is deduced from the precoding weights.
  ///
  /// The mapping into the resource grid does not precode the symbols, therefore, the resource grid is filled with the
  /// symbols corresponding to each layer.
  ///
  /// \param[in] symbols      Buffer containing the complex symbols to map.
  /// \param[in] i_symbol     OFDM symbol index within the slot.
  /// \param[in] i_subcarrier Initial subcarrier index.
  /// \param[in] mask         Allocation mask.
  /// \param[in] precoding    Precoding matrix weights.
  /// \remark The number of selected elements in \c mask must be equal to \c symbols size.
  /// \remark The size of \c symbols must be multiple of the number of layers.
  void map(span<const cf_t>                    symbols,
           unsigned                            i_symbol,
           unsigned                            i_subcarrier,
           const bounded_bitset<NRE * MAX_RB>& mask,
           const precoding_weight_matrix&      precoding) override;

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
