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

#include "srsran/phy/upper/signal_processors/pucch/factories.h"
#include "dmrs_pucch_estimator_format2.h"
#include "dmrs_pucch_estimator_formats3_4.h"
#include "dmrs_pucch_estimator_impl.h"

using namespace srsran;

namespace {

class dmrs_pucch_estimator_sw_factory : public dmrs_pucch_estimator_factory
{
public:
  dmrs_pucch_estimator_sw_factory(std::shared_ptr<pseudo_random_generator_factory>&      prg_factory_,
                                  std::shared_ptr<low_papr_sequence_collection_factory>& lpc_factory_,
                                  std::shared_ptr<low_papr_sequence_generator_factory>&  lpg_factory_,
                                  std::shared_ptr<port_channel_estimator_factory>&       ch_estimator_factory_) :
    prg_factory(std::move(prg_factory_)),
    lpc_factory(std::move(lpc_factory_)),
    lpg_factory(std::move(lpg_factory_)),
    ch_estimator_factory(std::move(ch_estimator_factory_))
  {
    srsran_assert(prg_factory, "Invalid sequence generator factory.");
    srsran_assert(lpc_factory, "Invalid sequence collection factory.");
    srsran_assert(lpg_factory, "Invalid sequence generator factory.");
    srsran_assert(ch_estimator_factory, "Invalid channel estimator factory.");
  }

  std::unique_ptr<dmrs_pucch_estimator> create() override
  {
    std::unique_ptr<dmrs_pucch_estimator_format2> estimator_format2 = std::make_unique<dmrs_pucch_estimator_format2>(
        prg_factory->create(),
        ch_estimator_factory->create(port_channel_estimator_fd_smoothing_strategy::filter,
                                     port_channel_estimator_td_interpolation_strategy::average,
                                     true));

    std::unique_ptr<dmrs_pucch_estimator_formats3_4> estimator_formats3_4 =
        std::make_unique<dmrs_pucch_estimator_formats3_4>(
            prg_factory->create(),
            lpg_factory->create(),
            ch_estimator_factory->create(port_channel_estimator_fd_smoothing_strategy::filter,
                                         port_channel_estimator_td_interpolation_strategy::average,
                                         /*compensate_cfo =*/false));

    return std::make_unique<dmrs_pucch_estimator_impl>(std::move(estimator_format2), std::move(estimator_formats3_4));
  }

private:
  std::shared_ptr<pseudo_random_generator_factory>      prg_factory;
  std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory;
  std::shared_ptr<low_papr_sequence_generator_factory>  lpg_factory;
  std::shared_ptr<port_channel_estimator_factory>       ch_estimator_factory;
};

} // namespace

std::shared_ptr<dmrs_pucch_estimator_factory>
srsran::create_dmrs_pucch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory>      prg_factory,
                                               std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory,
                                               std::shared_ptr<low_papr_sequence_generator_factory>  lpg_factory,
                                               std::shared_ptr<port_channel_estimator_factory> ch_estimator_factory)
{
  return std::make_shared<dmrs_pucch_estimator_sw_factory>(prg_factory, lpc_factory, lpg_factory, ch_estimator_factory);
}
