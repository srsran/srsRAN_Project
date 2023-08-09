/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Channel precoder interface.

#pragma once

#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/phy/support/re_buffer.h"

namespace srsran {

/// Channel precoder interface.
class channel_precoder
{
public:
  /// Default destructor.
  virtual ~channel_precoder() = default;

  /// \brief Applies a set of precoding weights to the input resource elements.
  ///
  /// \param[out] output   Output symbols, indexed by RE and antenna port.
  /// \param[in] input     Input symbols, indexed by RE and transmit layer.
  /// \param[in] precoding Precoding coefficients arranged in matrix form, indexed by transmit layer and antenna port.
  /// \remark An assertion is triggered if the output buffer does not have the same number of RE per port as the number
  /// of RE per layer of the input buffer.
  /// \remark An assertion is triggered if the precoding matrix dimensions do not match the number of layers of the
  /// input buffer and the number of antenna ports of the output buffer.
  virtual void apply_precoding(re_buffer_writer&              output,
                               const re_buffer_reader&        input,
                               const precoding_weight_matrix& precoding) = 0;

  /// \brief Applies precoding to the RE belonging to a single antenna port.
  ///
  /// \param[out] port_re   View over the RE of a single antenna port.
  /// \param[in] input     Input symbols, indexed by RE and transmit layer.
  /// \param[in] precoding Precoding coefficients, indexed by layer.
  virtual void
  apply_precoding_port(span<cf_t> port_re, const re_buffer_reader& input_re, span<const cf_t> port_weights) = 0;
};

} // namespace srsran
