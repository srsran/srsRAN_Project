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

#include "srsran/phy/upper/signal_processors/pusch/factories.h"
#include "dmrs_pusch_estimator_impl.h"

using namespace srsran;

namespace {

class dmrs_pusch_estimator_factory_sw : public dmrs_pusch_estimator_factory
{
public:
  dmrs_pusch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory>     prg_factory_,
                                  std::shared_ptr<low_papr_sequence_generator_factory> low_papr_gen_factory_,
                                  std::shared_ptr<port_channel_estimator_factory>      ch_estimator_factory_,
                                  task_executor&                                       executor_,
                                  port_channel_estimator_fd_smoothing_strategy         fd_smoothing_strategy_,
                                  port_channel_estimator_td_interpolation_strategy     td_interpolation_strategy_,
                                  bool                                                 compensate_cfo_) :
    prg_factory(std::move(prg_factory_)),
    low_papr_gen_factory(std::move(low_papr_gen_factory_)),
    ch_estimator_factory(std::move(ch_estimator_factory_)),
    executor(executor_),
    fd_smoothing_strategy(fd_smoothing_strategy_),
    td_interpolation_strategy(td_interpolation_strategy_),
    compensate_cfo(compensate_cfo_)
  {
    srsran_assert(prg_factory, "Invalid PRG factory.");
    srsran_assert(low_papr_gen_factory, "Invalid low-PAPR generator factory.");
    srsran_assert(ch_estimator_factory, "Invalid channel estimator factory.");
  }

  std::unique_ptr<dmrs_pusch_estimator> create() override
  {
    return std::make_unique<dmrs_pusch_estimator_impl>(
        prg_factory->create(),
        low_papr_gen_factory->create(),
        ch_estimator_factory->create(fd_smoothing_strategy, td_interpolation_strategy, compensate_cfo),
        executor);
  }

private:
  std::shared_ptr<pseudo_random_generator_factory>     prg_factory;
  std::shared_ptr<low_papr_sequence_generator_factory> low_papr_gen_factory;
  std::shared_ptr<port_channel_estimator_factory>      ch_estimator_factory;
  task_executor&                                       executor;
  port_channel_estimator_fd_smoothing_strategy         fd_smoothing_strategy;
  port_channel_estimator_td_interpolation_strategy     td_interpolation_strategy;
  bool                                                 compensate_cfo;
};

} // namespace

std::shared_ptr<dmrs_pusch_estimator_factory> srsran::create_dmrs_pusch_estimator_factory_sw(
    std::shared_ptr<pseudo_random_generator_factory>     prg_factory,
    std::shared_ptr<low_papr_sequence_generator_factory> low_papr_sequence_gen_factory,
    std::shared_ptr<port_channel_estimator_factory>      ch_estimator_factory,
    task_executor&                                       executor,
    port_channel_estimator_fd_smoothing_strategy         fd_smoothing_strategy,
    port_channel_estimator_td_interpolation_strategy     td_interpolation_strategy,
    bool                                                 compensate_cfo)
{
  return std::make_shared<dmrs_pusch_estimator_factory_sw>(std::move(prg_factory),
                                                           std::move(low_papr_sequence_gen_factory),
                                                           std::move(ch_estimator_factory),
                                                           executor,
                                                           fd_smoothing_strategy,
                                                           td_interpolation_strategy,
                                                           compensate_cfo);
}
