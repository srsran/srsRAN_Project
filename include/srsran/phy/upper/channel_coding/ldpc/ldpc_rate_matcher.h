/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC rate matching.
#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

class ldpc_encoder_buffer;

/// LDPC rate matching (i.e., bit selection and bit interleaving) interface.
class ldpc_rate_matcher
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_matcher() = default;

  /// \brief Carries out the rate matching of a codeblock.
  ///
  /// \param[out] output  Rate matched codeblock.
  /// \param[in]  input   Reference to an LDPC encoder buffer.
  /// \param[in]  cfg     Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate matching algorithm.
  virtual void rate_match(bit_buffer& output, const ldpc_encoder_buffer& input, const codeblock_metadata& cfg) = 0;
};

} // namespace srsran
