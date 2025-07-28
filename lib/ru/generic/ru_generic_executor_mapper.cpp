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
  task_executor& uplink_executor() override { return ul_exec; } // namespace
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
  ru_generic_executor_mapper_impl(ru_generic_executor_mapper_configuration& config) : async_exec(*config.radio_exec)
  {
    srsran_assert(config.radio_exec != nullptr, "Invalid radio executor.");
    for (auto& sector : config.sectors) {
      task_executor* dl_exec    = sector.dl_exec;
      task_executor* ul_exec    = sector.ul_exec;
      task_executor* prach_exec = sector.prach_exec;
      task_executor* tx_exec    = sector.tx_exec;
      task_executor* rx_exec    = sector.rx_exec;

      srsran_assert(dl_exec != nullptr, "Invalid downlink executor.");
      srsran_assert(ul_exec != nullptr, "Invalid uplink executor.");
      srsran_assert(prach_exec != nullptr, "Invalid PRACH executor.");
      srsran_assert(tx_exec != nullptr, "Invalid transmitter executor.");
      srsran_assert(rx_exec != nullptr, "Invalid receiver executor.");

      sectors.emplace_back(*dl_exec, *ul_exec, *prach_exec, *tx_exec, *rx_exec);
    }
  }

  ru_sdr_sector_executor_mapper& get_sector_mapper(unsigned index) override { return sectors[index]; }

  task_executor& asynchronous_radio_executor() override { return async_exec; }

private:
  std::vector<ru_sdr_sector_executor_mapper_impl> sectors;
  task_executor&                                  async_exec;
};

} // namespace

std::unique_ptr<ru_generic_executor_mapper>
srsran::create_ru_generic_executor_mapper(ru_generic_executor_mapper_configuration& config)
{
  return std::make_unique<ru_generic_executor_mapper_impl>(config);
}
