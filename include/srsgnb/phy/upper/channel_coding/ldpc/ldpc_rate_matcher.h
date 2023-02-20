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
/// \brief LDPC rate matching.
#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"

namespace srsgnb {

/// LDPC rate matching (i.e., bit selection and bit interleaving) interface.
class ldpc_rate_matcher
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_matcher() = default;

  /// \brief Carries out the rate matching of a codeblock.
  ///
  /// \param[out] output  Rate matched codeblock. Each \c uint8_t entry corresponds to a single bit.
  /// \param[in]  input   Original, full codeblock (may contain filler bits). Each \c uint8_t entry corresponds to a
  ///                     single bit.
  /// \param[in]  cfg     Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate matching algorithm.
  virtual void
  rate_match(span<uint8_t> output, span<const uint8_t> input, const codeblock_metadata::tb_common_metadata& cfg) = 0;
};

} // namespace srsgnb
