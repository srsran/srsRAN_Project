/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/signal_processors/channel_estimator/factories.h"
#include "port_channel_estimator_average_impl.h"
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

} // namespace

std::shared_ptr<port_channel_estimator_factory>
srsran::create_port_channel_estimator_factory_sw(std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory)
{
  return std::make_shared<port_channel_estimator_factory_sw>(std::move(ta_estimator_factory));
}
