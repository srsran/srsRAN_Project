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

#include "precoding_configuration.h"
#include "re_buffer.h"
#include "re_pattern.h"

namespace srsran {

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
    virtual span<const cf_t> pop_symbols(unsigned block_size) = 0;
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
  ///
  /// The resource mapping comprises layer mapping, precoding and copying onto the resource grid.
  ///
  /// The mapping is performed over the symbol index \c i_symbol, starting at subcarrier index \c i_subcarrier and for
  /// the subcarriers indicated by \c mask. The number of layers is deduced from the precoding weights.
  ///
  /// \param[in] symbols      Buffer containing the complex symbols to map.
  /// \param[in] i_symbol     OFDM symbol index within the slot.
  /// \param[in] i_subcarrier Initial subcarrier index.
  /// \param[in] mask         Allocation mask.
  /// \param[in] precoding    Precoding matrix weights.
  /// \remark The number of selected elements in \c mask must be equal to \c symbols size.
  /// \remark The size of \c symbols must be multiple of the number of layers.
  virtual void map(symbol_buffer&                 buffer,
                   const re_pattern_list&         pattern,
                   const re_pattern_list&         reserved,
                   const precoding_configuration& precoding) = 0;
};

} // namespace srsran
