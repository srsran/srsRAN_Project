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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/support/interpolator.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator_parameters.h"

namespace srsran {

/// \brief Applies frequency domain smoothing strategy.
/// \param[out] enlarged_filtered_pilots_out   Smoothed pilots estimates.
/// \param[in]  enlarged_pilots_in             Pilots estimates.
/// \param[in]  nof_rb                         Number of resource blocks.
/// \param[in]  stride                         Reference signals stride in frequency domain.
/// \param[in]  fd_smoothing_strategy          Frequency domain smoothing strategy.
void apply_fd_smoothing(span<cf_t>                                   enlarged_filtered_pilots_out,
                        span<cf_t>                                   enlarged_pilots_in,
                        unsigned                                     nof_rb,
                        unsigned                                     stride,
                        port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy);

/// \brief Estimates the time alignment based on one hop.
///
/// \param[in] pilots_lse   The estimated channel (only for REs carrying DM-RS).
/// \param[in] pattern      DM-RS pattern for the current layer.
/// \param[in] hop          Intra-slot frequency hopping index: 0 for first position (before hopping), 1 for second
///                         position (after hopping).
/// \param[in] scs          Subcarrier spacing.
/// \param[in] ta_estimator Time alignment estimator.
/// \return The estimated time alignment as a number of samples (the sampling frequency is given by the DFT processor).
float estimate_time_alignment(const re_buffer_reader<cf_t>&                     pilots_lse,
                              const port_channel_estimator::layer_dmrs_pattern& pattern,
                              unsigned                                          hop,
                              subcarrier_spacing                                scs,
                              time_alignment_estimator&                         ta_estimator);

// Returns the interpolator configuration for the given RE pattern.
interpolator::configuration configure_interpolator(const bounded_bitset<NRE>& re_mask);
} // namespace srsran
