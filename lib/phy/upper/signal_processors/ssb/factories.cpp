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

#include "srsran/phy/upper/signal_processors/ssb/factories.h"
#include "dmrs_pbch_processor_impl.h"
#include "pss_processor_impl.h"
#include "sss_processor_impl.h"

using namespace srsran;

namespace {

class dmrs_pbch_processor_sw_factory : public dmrs_pbch_processor_factory
{
private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  explicit dmrs_pbch_processor_sw_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    prg_factory(std::move(prg_factory_))
  {
    srsran_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<dmrs_pbch_processor> create() override
  {
    return std::make_unique<dmrs_pbch_processor_impl>(prg_factory->create());
  }
};

class pss_processor_factory_sw : public pss_processor_factory
{
public:
  std::unique_ptr<pss_processor> create() override { return std::make_unique<pss_processor_impl>(); }
};

class sss_processor_factory_sw : public sss_processor_factory
{
public:
  std::unique_ptr<sss_processor> create() override { return std::make_unique<sss_processor_impl>(); }
};

} // namespace

std::shared_ptr<dmrs_pbch_processor_factory>
srsran::create_dmrs_pbch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<dmrs_pbch_processor_sw_factory>(std::move(prg_factory));
}

std::shared_ptr<pss_processor_factory> srsran::create_pss_processor_factory_sw()
{
  return std::make_shared<pss_processor_factory_sw>();
}

std::shared_ptr<sss_processor_factory> srsran::create_sss_processor_factory_sw()
{
  return std::make_shared<sss_processor_factory_sw>();
}
