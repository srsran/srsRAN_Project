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
