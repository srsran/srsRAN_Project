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

  /// \brief Maps complex symbols into the resource grid.
  ///
  /// The resource mapping comprises layer mapping, precoding and copying into the resource grid.
  ///
  /// The mapping is performed over the symbol index \c i_symbol, starting at subcarrier index \c i_subcarrier and for
  /// the subcarriers indicated by \c mask. The number of layers is deduced from the precoding weights.
  ///
  /// \param[in] i_symbol     OFDM symbol index within the slot.
  /// \param[in] i_subcarrier Initial subcarrier index.
  /// \param[in] mask         Allocation mask.
  /// \param[in] precoding    Precoding matrix weights.
  /// \param[in] symbols      Buffer containing the complex symbols to map.
  /// \remark The number of selected elements in \c mask must be equal to \c symbols size.
  /// \remark The size of \c symbols must be multiple of the number of layers.
  virtual void map(unsigned                            i_symbol,
                   unsigned                            i_subcarrier,
                   const bounded_bitset<NRE * MAX_RB>& mask,
                   const precoding_weight_matrix&      precoding,
                   span<const cf_t>                    symbols) = 0;
};

} // namespace srsran
