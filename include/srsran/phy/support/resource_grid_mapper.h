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
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class re_buffer_reader;
class re_pattern_list;
class precoding_configuration;

/// \brief Resource Element mapping interface.
///
/// Provides a generic interface that DL channel processors use to map Resource Elements into the resource grid.
class resource_grid_mapper
{
public:
  /// Default destructor.
  virtual ~resource_grid_mapper() = default;

  /// Complex symbol buffer.
  class symbol_buffer
  {
  public:
    /// Default destructor.
    virtual ~symbol_buffer() = default;

    /// Gets the maximum block size.
    virtual unsigned get_max_block_size() const = 0;

    /// Pops \c block_size number of symbols.
    virtual span<const ci8_t> pop_symbols(unsigned block_size) = 0;
  };

  /// Adapts an existing symbol vector to a symbol buffer interface.
  class symbol_buffer_adapter : public symbol_buffer
  {
  public:
    /// Creates an adapter based on the view of a data view.
    symbol_buffer_adapter(span<const ci8_t> symbols_) : symbols(symbols_) {}

    /// Destructor. It triggers an assertion if the buffer is not empty.
    ~symbol_buffer_adapter() { srsran_assert(symbols.empty(), "{} symbols are still in the buffer.", symbols.size()); }

    // See interface for documentation.
    unsigned get_max_block_size() const override { return symbols.size(); }

    // See interface for documentation.
    span<const ci8_t> pop_symbols(unsigned block_size) override
    {
      // Make sure the block size does not exceed the number of symbols.
      srsran_assert(symbols.size() >= block_size,
                    "The block size (i.e., {}) exceeds the number of available symbols (i.e., {}).",
                    block_size,
                    symbols.size());

      // Select view of the symbols to return.
      span<const ci8_t> ret = symbols.first(block_size);

      // Cut symbols.
      symbols = symbols.last(symbols.size() - block_size);

      return ret;
    }

  private:
    span<const ci8_t> symbols;
  };

  /// \brief Maps the input resource elements into the resource grid.
  /// \param[in] input      Input data.
  /// \param[in] pattern    Data allocation pattern in the resource grid.
  /// \param[in] precoding  Precoding configuration.
  virtual void
  map(const re_buffer_reader& input, const re_pattern_list& pattern, const precoding_configuration& precoding) = 0;

  /// \brief Maps the input resource elements into the resource grid.
  /// \param[in] input      Input data.
  /// \param[in] pattern    Data allocation pattern in the resource grid.
  /// \param[in] reserved   Reserved resource elements, to be excluded from the allocation pattern.
  /// \param[in] precoding  Precoding configuration.
  virtual void map(const re_buffer_reader&        input,
                   const re_pattern_list&         pattern,
                   const re_pattern_list&         reserved,
                   const precoding_configuration& precoding) = 0;

  /// \brief Maps complex symbols onto the resource grid.
  /// \param[in] buffer     Buffer containing the complex symbols to map.
  /// \param[in] pattern    Data allocation pattern in the resource grid.
  /// \param[in] reserved   Reserved resource elements, to be excluded from the allocation pattern.
  /// \param[in] precoding  Precoding configuration.
  virtual void map(symbol_buffer&                 buffer,
                   const re_pattern_list&         pattern,
                   const re_pattern_list&         reserved,
                   const precoding_configuration& precoding) = 0;
};

} // namespace srsran
