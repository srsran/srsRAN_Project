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

#include "srsran/ru/generic/ru_generic_executor_mapper.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

namespace {

class ru_sdr_sector_executor_mapper_impl : public ru_sdr_sector_executor_mapper
{
public:
  ru_sdr_sector_executor_mapper_impl(task_executor& dl_exec_,
                                     task_executor& ul_exec_,
                                     task_executor& prach_exec_,
                                     task_executor& tx_exec_,
                                     task_executor& rx_exec_) :

    dl_exec(dl_exec_), ul_exec(ul_exec_), prach_exec(prach_exec_), tx_exec(tx_exec_), rx_exec(rx_exec_)
  {
  }

  task_executor& downlink_executor() override { return dl_exec; }
  task_executor& uplink_executor() override { return ul_exec; }
  task_executor& prach_executor() override { return prach_exec; }
  task_executor& transmitter_executor() override { return tx_exec; }
  task_executor& receiver_executor() override { return rx_exec; }

private:
  task_executor& dl_exec;
  task_executor& ul_exec;
  task_executor& prach_exec;
  task_executor& tx_exec;
  task_executor& rx_exec;
};

class ru_generic_executor_mapper_impl : public ru_generic_executor_mapper
{
public:
  ru_generic_executor_mapper_impl(const ru_generic_executor_mapper_sequential_configuration& config)
  {
    srsran_assert(config.asynchronous_exec != nullptr, "Invalid asynchronous executor.");
    srsran_assert(config.common_exec != nullptr, "Invalid common executor.");

    async_exec = config.asynchronous_exec;

    for (unsigned i_sector = 0; i_sector != config.nof_sectors; ++i_sector) {
      task_executor& dl_exec    = inline_executor;
      task_executor& ul_exec    = inline_executor;
      task_executor& prach_exec = inline_executor;
      task_executor& tx_exec    = *config.common_exec;
      task_executor& rx_exec    = *config.common_exec;
      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  ru_generic_executor_mapper_impl(const ru_generic_executor_mapper_single_configuration& config)
  {
    srsran_assert(config.radio_exec != nullptr, "Invalid radio executor.");
    srsran_assert(config.high_prio_executor != nullptr, "Invalid high priority executor.");

    async_exec = config.radio_exec;

    for (auto& baseband_exec : config.baseband_exec) {
      srsran_assert(baseband_exec != nullptr, "Invalid baseband cell executor.");

      task_executor& dl_exec    = *config.high_prio_executor;
      task_executor& ul_exec    = inline_executor;
      task_executor& prach_exec = *config.high_prio_executor;
      task_executor& tx_exec    = *baseband_exec;
      task_executor& rx_exec    = *baseband_exec;

      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  ru_generic_executor_mapper_impl(const ru_generic_executor_mapper_dual_configuration& config)
  {
    srsran_assert(config.radio_exec != nullptr, "Invalid radio executor.");
    srsran_assert(config.high_prio_executor != nullptr, "Invalid high priority executor.");

    async_exec = config.radio_exec;

    for (auto& baseband_exec : config.baseband_exec) {
      srsran_assert(baseband_exec.rx_exec != nullptr, "Invalid receive baseband cell executor.");
      srsran_assert(baseband_exec.tx_exec != nullptr, "Invalid transmit baseband cell executor.");

      task_executor& dl_exec    = *config.high_prio_executor;
      task_executor& ul_exec    = inline_executor;
      task_executor& prach_exec = *config.high_prio_executor;
      task_executor& tx_exec    = *baseband_exec.tx_exec;
      task_executor& rx_exec    = *baseband_exec.rx_exec;

      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  ru_generic_executor_mapper_impl(const ru_generic_executor_mapper_triple_configuration& config)
  {
    srsran_assert(config.radio_exec != nullptr, "Invalid radio executor.");
    srsran_assert(config.high_prio_executor != nullptr, "Invalid high priority executor.");

    async_exec = config.radio_exec;

    for (auto& baseband_exec : config.baseband_exec) {
      srsran_assert(baseband_exec.rx_exec != nullptr, "Invalid receive baseband cell executor.");
      srsran_assert(baseband_exec.tx_exec != nullptr, "Invalid transmit baseband cell executor.");
      srsran_assert(baseband_exec.ul_exec != nullptr, "Invalid baseband demodulator cell executor.");

      task_executor& dl_exec    = *config.high_prio_executor;
      task_executor& ul_exec    = *baseband_exec.ul_exec;
      task_executor& prach_exec = *config.high_prio_executor;
      task_executor& tx_exec    = *baseband_exec.tx_exec;
      task_executor& rx_exec    = *baseband_exec.rx_exec;

      sectors.emplace_back(dl_exec, ul_exec, prach_exec, tx_exec, rx_exec);
    }
  }

  ru_sdr_sector_executor_mapper& get_sector_mapper(unsigned index) override { return sectors[index]; }

  task_executor& asynchronous_radio_executor() override { return *async_exec; }

private:
  std::vector<ru_sdr_sector_executor_mapper_impl> sectors;
  task_executor*                                  async_exec;
  inline_task_executor                            inline_executor;
};

} // namespace

std::unique_ptr<ru_generic_executor_mapper>
srsran::create_ru_generic_executor_mapper(const ru_generic_executor_mapper_sequential_configuration& config)
{
  return std::make_unique<ru_generic_executor_mapper_impl>(config);
}

std::unique_ptr<ru_generic_executor_mapper>
srsran::create_ru_generic_executor_mapper(const srsran::ru_generic_executor_mapper_single_configuration& config)
{
  return std::make_unique<ru_generic_executor_mapper_impl>(config);
}

std::unique_ptr<ru_generic_executor_mapper>
srsran::create_ru_generic_executor_mapper(const srsran::ru_generic_executor_mapper_dual_configuration& config)
{
  return std::make_unique<ru_generic_executor_mapper_impl>(config);
}

std::unique_ptr<ru_generic_executor_mapper>
srsran::create_ru_generic_executor_mapper(const srsran::ru_generic_executor_mapper_triple_configuration& config)
{
  return std::make_unique<ru_generic_executor_mapper_impl>(config);
}
