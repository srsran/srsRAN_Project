/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
using channel_precoder_metric_notifier   = detail::phy_metric_notifier<channel_precoder_metrics>;
using channel_equalizer_metric_notifier  = detail::phy_metric_notifier<channel_equalizer_metrics>;
using channel_modulation_metric_notifier = detail::phy_metric_notifier<channel_modulation_metrics>;

/// Groups upper physical layer metric notifiers.
class upper_phy_metrics_notifiers
{
public:
  virtual ~upper_phy_metrics_notifiers()                                                             = default;
  virtual crc_calculator_metric_notifier&                   get_pdsch_crc_calculator_notifier()      = 0;
  virtual crc_calculator_metric_notifier&                   get_pusch_crc_calculator_notifier()      = 0;
  virtual ldpc_encoder_metric_notifier&                     get_ldpc_encoder_notifier()              = 0;
  virtual ldpc_decoder_metric_notifier&                     get_ldpc_decoder_notifier()              = 0;
  virtual pusch_channel_estimator_metric_notifier&          get_pusch_channel_estimator_notifier()   = 0;
  virtual pusch_processor_metric_notifier&                  get_pusch_processor_notifier()           = 0;
  virtual pseudo_random_sequence_generator_metric_notifier& get_pusch_scrambling_notifier()          = 0;
  virtual pseudo_random_sequence_generator_metric_notifier& get_pdsch_scrambling_notifier()          = 0;
  virtual channel_precoder_metric_notifier&                 get_pdsch_channel_precoder_notifier()    = 0;
  virtual channel_equalizer_metric_notifier&                get_pusch_channel_equalizer_notifier()   = 0;
  virtual channel_modulation_metric_notifier&               get_pdsch_modulation_mapper_notifier()   = 0;
  virtual channel_modulation_metric_notifier&               get_pusch_demodulation_mapper_notifier() = 0;
  virtual channel_modulation_metric_notifier&               get_pusch_evm_calculator_notifier()      = 0;
};

} // namespace srsran
