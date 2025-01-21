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

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsran {

/// Creates an LDPC decoder metric decorator factory.
std::shared_ptr<ldpc_decoder_factory>
create_ldpc_decoder_metric_decorator_factory(std::shared_ptr<ldpc_decoder_factory> base_factory,
                                             ldpc_decoder_metric_notifier&         notifier);

/// Creates a PUSCH channel processor metric decorator factory.
std::shared_ptr<dmrs_pusch_estimator_factory>
create_pusch_channel_estimator_metric_decorator_factory(std::shared_ptr<dmrs_pusch_estimator_factory> base_factory,
                                                        pusch_channel_estimator_metric_notifier&      notifier);

/// Creates PUSCH processor metric decorator factory.
std::shared_ptr<pusch_processor_factory>
create_pusch_processor_metric_decorator_factory(std::shared_ptr<pusch_processor_factory> base_factory,
                                                pusch_processor_metric_notifier&         notifier);

} // namespace srsran
