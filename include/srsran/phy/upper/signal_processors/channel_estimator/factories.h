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

#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"
#include "srsran/phy/upper/signal_processors/channel_estimator/port_channel_estimator.h"
#include "srsran/phy/upper/signal_processors/channel_estimator/port_channel_estimator_parameters.h"

namespace srsran {

class port_channel_estimator_factory
{
public:
  virtual ~port_channel_estimator_factory() = default;
  virtual std::unique_ptr<port_channel_estimator>
  create(port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
         port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
         bool                                             complensate_cfo) = 0;
};

std::shared_ptr<port_channel_estimator_factory>
create_port_channel_estimator_factory_sw(std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory);

std::shared_ptr<port_channel_estimator_factory>
create_port_channel_estimator_pool_factory(std::shared_ptr<port_channel_estimator_factory> ch_est_factory,
                                           unsigned                                        nof_concurrent_threads);

} // namespace srsran
