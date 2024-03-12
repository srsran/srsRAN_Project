/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_measurement.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// Time alignment estimator interface.
class time_alignment_estimator
{
public:
  /// \brief Maximum number of symbols that can be used for estimating the time alignment.
  ///
  /// It is equal to the maximum number of subcarriers that can be contained in an OFDM symbol.
  static constexpr unsigned max_nof_symbols = MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB;

  /// Default destructor.
  virtual ~time_alignment_estimator() = default;

  /// \brief Estimates the time alignment from frequency domain symbols.
  /// \param[in] symbols Complex frequency domain symbols.
  /// \param[in] mask    Distribution of the complex symbols within an OFDM symbol.
  /// \param[in] scs     Subcarrier spacing.
  /// \param[in] max_ta  Maximum absolute time alignment measurement if it is not zero.
  /// \return The measured time alignment.
  /// \remark An assertion is triggered if the number of symbols is not equal to the number of active elements in the
  /// mask.
  virtual time_alignment_measurement estimate(span<const cf_t>                symbols,
                                              bounded_bitset<max_nof_symbols> mask,
                                              subcarrier_spacing              scs,
                                              double                          max_ta = 0.0) = 0;
};

} // namespace srsran
