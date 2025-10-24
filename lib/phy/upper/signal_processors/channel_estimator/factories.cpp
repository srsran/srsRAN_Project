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

#include "srsran/phy/upper/signal_processors/channel_estimator/factories.h"
#include "port_channel_estimator_average_impl.h"
#include "port_channel_estimator_pool.h"
#include "srsran/phy/support/support_factories.h"

using namespace srsran;

namespace {

class port_channel_estimator_factory_sw : public port_channel_estimator_factory
{
public:
  explicit port_channel_estimator_factory_sw(std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory_) :
    ta_estimator_factory(std::move(ta_estimator_factory_))
  {
    srsran_assert(ta_estimator_factory, "Invalid TA estimator factory.");
  }

  std::unique_ptr<port_channel_estimator>
  create(port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
         port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
         bool                                             compensate_cfo) override
  {
    std::unique_ptr<interpolator> interp = create_interpolator();

    return std::make_unique<port_channel_estimator_average_impl>(std::move(interp),
                                                                 ta_estimator_factory->create(),
                                                                 fd_smoothing_strategy,
                                                                 td_interpolation_strategy,
                                                                 compensate_cfo);
  }

private:
  std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory;
};

class port_channel_estimator_pool_factory : public port_channel_estimator_factory
{
public:
  port_channel_estimator_pool_factory(std::shared_ptr<port_channel_estimator_factory> factory_,
                                      unsigned                                        nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(factory, "Invalid port channel estimator factory.");
    srsran_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  std::unique_ptr<port_channel_estimator>
  create(port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
         port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
         bool                                             compensate_cfo) override
  {
    auto& this_estimator_pool = estimators[{fd_smoothing_strategy, td_interpolation_strategy, compensate_cfo}];
    if (!this_estimator_pool) {
      std::vector<std::unique_ptr<port_channel_estimator>> instances(nof_concurrent_threads);
      std::generate(instances.begin(),
                    instances.end(),
                    [this, fd_smoothing_strategy, td_interpolation_strategy, compensate_cfo]() {
                      return factory->create(fd_smoothing_strategy, td_interpolation_strategy, compensate_cfo);
                    });
      this_estimator_pool = std::make_shared<port_channel_estimator_pool::estimator_pool>(instances);
    }
    return std::make_unique<port_channel_estimator_pool>(this_estimator_pool);
  }

private:
  using estimator_config =
      std::tuple<port_channel_estimator_fd_smoothing_strategy, port_channel_estimator_td_interpolation_strategy, bool>;

  struct config_hash {
    size_t operator()(const estimator_config& c) const noexcept
    {
      size_t h1 = std::hash<int>{}(static_cast<int>(std::get<0>(c)));
      size_t h2 = std::hash<int>{}(static_cast<int>(std::get<1>(c)));
      size_t h3 = std::hash<bool>{}(std::get<2>(c));

      return (h1 ^ (h2 << 1) ^ (h3 << 2));
    }
  };

  std::shared_ptr<port_channel_estimator_factory> factory;
  unsigned                                        nof_concurrent_threads;
  std::unordered_map<estimator_config, std::shared_ptr<port_channel_estimator_pool::estimator_pool>, config_hash>
      estimators;
};

} // namespace

std::shared_ptr<port_channel_estimator_factory>
srsran::create_port_channel_estimator_factory_sw(std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory)
{
  return std::make_shared<port_channel_estimator_factory_sw>(std::move(ta_estimator_factory));
}

std::shared_ptr<port_channel_estimator_factory>
srsran::create_port_channel_estimator_pool_factory(std::shared_ptr<port_channel_estimator_factory> ch_est_factory,
                                                   unsigned nof_concurrent_threads)
{
  return std::make_shared<port_channel_estimator_pool_factory>(std::move(ch_est_factory), nof_concurrent_threads);
}
