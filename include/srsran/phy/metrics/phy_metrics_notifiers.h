/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/metrics/phy_metrics_reports.h"

namespace srsran {

namespace detail {

/// \brief Generic physical layer metric notifier
/// \tparam Metric Metric type.
template <typename Metrics>
class phy_metric_notifier
{
public:
  /// Default destructor.
  virtual ~phy_metric_notifier() = default;

  /// Notifies a new metric collection.
  virtual void new_metric(const Metrics& metrics) = 0;
};

} // namespace detail

using crc_calculator_metric_notifier          = detail::phy_metric_notifier<crc_calculator_metrics>;
using ldpc_encoder_metric_notifier            = detail::phy_metric_notifier<ldpc_encoder_metrics>;
using ldpc_decoder_metric_notifier            = detail::phy_metric_notifier<ldpc_decoder_metrics>;
using pusch_processor_metric_notifier         = detail::phy_metric_notifier<pusch_processor_metrics>;
using pusch_channel_estimator_metric_notifier = detail::phy_metric_notifier<pusch_channel_estimator_metrics>;
using pseudo_random_sequence_generator_metric_notifier =
    detail::phy_metric_notifier<pseudo_random_sequence_generator_metrics>;
using channel_precoder_metric_notifier  = detail::phy_metric_notifier<channel_precoder_metrics>;
using channel_equalizer_metric_notifier = detail::phy_metric_notifier<channel_equalizer_metrics>;

/// Groups upper physical layer metric notifiers.
class upper_phy_metrics_notifiers
{
public:
  virtual ~upper_phy_metrics_notifiers()                                                           = default;
  virtual crc_calculator_metric_notifier&                   get_pdsch_crc_calculator_notifier()    = 0;
  virtual crc_calculator_metric_notifier&                   get_pusch_crc_calculator_notifier()    = 0;
  virtual ldpc_encoder_metric_notifier&                     get_ldpc_encoder_notifier()            = 0;
  virtual ldpc_decoder_metric_notifier&                     get_ldpc_decoder_notifier()            = 0;
  virtual pusch_channel_estimator_metric_notifier&          get_pusch_channel_estimator_notifier() = 0;
  virtual pusch_processor_metric_notifier&                  get_pusch_processor_notifier()         = 0;
  virtual pseudo_random_sequence_generator_metric_notifier& get_pusch_scrambling_notifier()        = 0;
  virtual pseudo_random_sequence_generator_metric_notifier& get_pdsch_scrambling_notifier()        = 0;
  virtual channel_precoder_metric_notifier&                 get_pdsch_channel_precoder_notifier()  = 0;
  virtual channel_equalizer_metric_notifier&                get_pusch_channel_equalizer_notifier() = 0;
};

} // namespace srsran
